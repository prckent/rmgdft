/************************** SVN Revision Information **************************
 **    $Id$    **
 ******************************************************************************/

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>




#include "params.h"
#include "rmgtypedefs.h"
#include "typedefs.h"
#include "RmgTimer.h"

#include "prototypes_on.h"
#include "init_var.h"
#include "transition.h"
#include "blas.h"
#include "Kbpsi.h"

#include "BaseThread.h"
#include "rmgthreads.h"
#include "RmgThread.h"
#include "LdaU_on.h"


//template void CalculateResidual(LocalObject<std::complex<double>> &Phi, LocalObject<std::complex<double_> &H_Phi,
//LocalObject<std::complex<double>> &NlProj, 
//    double *vtot_c, std::complex<double> *theta_glob, std::complex<double> *kbpsi_glob);
//template void CalculateResidual(LocalObject<double> &Phi, LocalObject<double> &H_Phi, LocalObject<double> &NlProj, double *vtot_c, double *theta_glob, double *kbpsi_glob);
void CalculateResidual(LocalObject<double> &Phi, LocalObject<double> &H_Phi, 
        LocalObject<double> &NlProj, double *vtot_c, double *theta_glob, double *kbpsi_glob)
{

    FiniteDiff FD(&Rmg_L);

    RmgTimer *RT = new RmgTimer("3-OrbitalOptimize: calculate");


    double one = 1.0, zero = 0.0, mtwo = -2.0;


    int pbasis = Rmg_G->get_P0_BASIS(1);


    /* Loop over states */
    /* calculate the H |phi> on this processor and stored in states1[].psiR[] */

    for(int st1 = 0; st1 < Phi.num_thispe; st1++)
    {
        double *a_phi = &Phi.storage_proj[st1 * pbasis];
        double *h_phi = &H_Phi.storage_proj[st1 * pbasis];
        ApplyAOperator (a_phi, h_phi, "Coarse");

        for (int idx = 0; idx < pbasis; idx++)
        {
            h_phi[idx] = a_phi[idx] * vtot_c[idx] - 0.5 * h_phi[idx];
        }
    }


    if(ct.num_ldaU_ions > 0 )
        ldaU_on->app_vhubbard(H_Phi, *Rmg_G);

    double *theta_local = new double[Phi.num_thispe * Phi.num_thispe];
    double *kbpsi_local = new double[NlProj.num_thispe * Phi.num_thispe];
    double *kbpsi_work = new double[NlProj.num_thispe * Phi.num_thispe];
    double *kbpsi_work1 = new double[NlProj.num_thispe * Phi.num_thispe];


    mat_global_to_local (Phi, Phi, theta_glob, theta_local);
    mat_global_to_local (NlProj, Phi, kbpsi_glob, kbpsi_local);

    // calculate residual part H_Phi_j += Phi_j * Theta_ji
    //  Theta = S^-1H, 
    //
    int num_orb = Phi.num_thispe;
    int num_prj = NlProj.num_thispe;
    dgemm("N", "N", &pbasis, &num_orb, &num_orb,  &one, Phi.storage_proj, &pbasis,
            theta_local, &num_orb, &mtwo, H_Phi.storage_proj, &pbasis);


    //  kbpsi_work_m,i = <beta_m|phi_j> Theta_ji 
    dgemm("N", "N", &num_prj, &num_orb, &num_orb,  &one, kbpsi_local, &num_prj,
            theta_local, &num_orb, &zero, kbpsi_work, &num_prj);


    // assigin qnm and dnm for all ions into matrix

    double *dnmI, *qnmI;
    double *dnm, *qnm;

    dnm = new double[num_prj * num_prj];
    qnm = new double[num_prj * num_prj];

    for(int idx = 0; idx < num_prj * num_prj; idx++) 
    {
        dnm[idx] = 0.0;
        qnm[idx] = 0.0;
    }

    int proj_count = 0;
    int proj_count_local = 0;
    for (int ion = 0; ion < ct.num_ions; ion++)
    {
        ION *iptr = &Atoms[ion];
        SPECIES *sp = &Species[iptr->species];
        int nh = sp->num_projectors;
        if(nh == 0) continue;
        if(NlProj.index_global_to_proj[proj_count] >= 0) 
        {


            dnmI = pct.dnmI[ion];
            qnmI = pct.qqq[ion];

            for(int i = 0; i < nh; i++)
                for(int j = 0; j < nh; j++)
                {
                    int ii = i + proj_count_local;
                    int jj = j + proj_count_local;
                    dnm[ii *num_prj + jj] = dnmI[i * nh + j];
                    qnm[ii *num_prj + jj] = qnmI[i * nh + j];
                }
            proj_count_local += nh;
        }

        proj_count += nh;
    }

    assert(proj_count_local==num_prj);

    //  qnm * <beta_m|phi_j> theta_ji
    dgemm("N", "N", &num_prj, &num_orb, &num_prj,  &one, qnm, &num_prj, kbpsi_work, &num_prj,
            &zero, kbpsi_work1, &num_prj);
    //  dnm * <beta_m|phi_j> 
    dgemm("N", "N", &num_prj, &num_orb, &num_prj,  &mtwo, dnm, &num_prj, kbpsi_local, &num_prj,
            &one, kbpsi_work1, &num_prj);

    // |beta_n> * (qnm <beta|phi>theta + dnm <beta|phi>

    dgemm ("N", "N", &pbasis, &num_orb, &num_prj, &one, NlProj.storage_proj, &pbasis, 
            kbpsi_work1, &num_prj, &one, H_Phi.storage_proj, &pbasis);



    delete [] qnm;
    delete [] dnm;
    delete [] kbpsi_work1;;
    delete [] kbpsi_work;;
    delete [] kbpsi_local;
    delete [] theta_local;

    delete(RT);

} 



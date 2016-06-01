/************************** SVN Revision Information **************************
 **    $Id$    **
******************************************************************************/

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "make_conf.h"
#include "const.h"
#include "grid.h"
#include "rmgtypedefs.h"
#include "typedefs.h"
#include <complex>
#include "Kpoint.h"
#include "common_prototypes.h"
#include "common_prototypes1.h"
#include "transition.h"
#include "AtomicInterpolate.h"

static double harmonic_func_00(double r, double *b);
static double harmonic_func_10(double r, double *b);
static double harmonic_func_11(double r, double *b);
static double harmonic_func_12(double r, double *b);
static double harmonic_func_20(double r, double *b);
static double harmonic_func_21(double r, double *b);
static double harmonic_func_22(double r, double *b);
static double harmonic_func_23(double r, double *b);
static double harmonic_func_24(double r, double *b);
static double harmonic_func_30(double r, double *b);
static double harmonic_func_31(double r, double *b);
static double harmonic_func_32(double r, double *b);
static double harmonic_func_33(double r, double *b);
static double harmonic_func_34(double r, double *b);
static double harmonic_func_35(double r, double *b);
static double harmonic_func_36(double r, double *b);
void InitWeightOne (SPECIES * sp, fftw_complex * rtptr, int ip, int l, int m, fftw_plan p1)
{

    int idx, size;
    double r, ax[3], bx[3];
    double t1;
    std::complex<double> *weptr, *gwptr;
    double (*func) (double, double *) = NULL;

    switch(l)
    {
        case S_STATE: 
            func = harmonic_func_00;
            break;
        case P_STATE:
            if(m == 0) func = harmonic_func_10;
            if(m == 1) func = harmonic_func_12;
            if(m == 2) func = harmonic_func_11;
            break;
        case D_STATE:
            if(m == 0) func = harmonic_func_20;
            if(m == 1) func = harmonic_func_21;
            if(m == 2) func = harmonic_func_22;
            if(m == 3) func = harmonic_func_23;
            if(m == 4) func = harmonic_func_24;
            break;
        case F_STATE:
            if(m == 0) func = harmonic_func_30;
            if(m == 1) func = harmonic_func_31;
            if(m == 2) func = harmonic_func_32;
            if(m == 3) func = harmonic_func_33;
            if(m == 4) func = harmonic_func_34;
            if(m == 5) func = harmonic_func_35;
            if(m == 6) func = harmonic_func_36;
            break;
        default:
            printf("\n projecotr with ip = %d not programed \n", ip);
            exit(0);
    }




    // define functions to distiguish s, px, py, pz, ....

    double hxx = get_hxgrid() / (double) ct.nxfgrid;
    double hyy = get_hygrid() / (double) ct.nyfgrid;
    double hzz = get_hzgrid() / (double) ct.nzfgrid;
    double xside = get_xside();
    double yside = get_yside();
    double zside = get_zside();

    int nlfxdim = sp->nlfdim;
    int nlfydim = sp->nlfdim;
    int nlfzdim = sp->nlfdim;
    if(!ct.localize_projectors) {
        nlfxdim = ct.nxfgrid * get_NX_GRID();
        nlfydim = ct.nxfgrid * get_NY_GRID();
        nlfzdim = ct.nxfgrid * get_NZ_GRID();
    }

    /* nl[xyz]fdim is the size of the non-local box in the high density grid */
    size = nlfxdim * nlfydim * nlfzdim;

    weptr = new std::complex<double>[size];
    gwptr = new std::complex<double>[size];


    // Next we get the radius of the projectors in terms of grid points
    int dimx = 0;
    int dimy = 0;
    int dimz = 0;
    if(!ct.localize_projectors) {
        dimx = sp->nlradius / (hxx*xside);
        dimx = 2*(dimx/2);
        dimy = sp->nlradius / (hyy*yside);
        dimy = 2*(dimy/2);
        dimz = sp->nlradius / (hzz*zside);
        dimz = 2*(dimz/2);
    }

    // We assume that ion is in the center of non-local box for the localized
    // projector case. For the non-localized case it does not matter as long as
    // usage is consistent here and in GetPhase.cpp
    int ixbegin = -nlfxdim/2;
    int ixend = ixbegin + nlfxdim;
    int iybegin = -nlfydim/2;
    int iyend = iybegin + nlfydim;
    int izbegin = -nlfzdim/2;
    int izend = izbegin + nlfzdim;
    if(!ct.localize_projectors) {
        ixbegin = -dimx/2;
        ixend = ixbegin + dimx;
        iybegin = -dimy/2;
        iyend = iybegin + dimy;
        izbegin = -dimz/2;
        izend = izbegin + dimz;
    }


    for (int ix = ixbegin; ix < ixend; ix++)
    {
        int ixx = (ix + 20 * nlfxdim) % nlfxdim;

        double xc = (double) ix *hxx;

        for (int iy = iybegin; iy < iyend; iy++)
        {
            int iyy = (iy + 20 * nlfydim) % nlfydim;
            double yc = (double) iy *hyy;

            for (int iz = izbegin; iz < izend; iz++)
            {

                int izz = (iz + 20 * nlfzdim) % nlfzdim;
                double zc = (double) iz *hzz;

                ax[0] = xc;
                ax[1] = yc;
                ax[2] = zc;

                r = metric (ax);
                to_cartesian (ax, bx);
                r += 1.0e-10;

                t1 = AtomicInterpolateInline(&sp->betalig[ip][0], r);
                idx = ixx * nlfydim * nlfzdim + iyy * nlfzdim + izz;
                weptr[idx] = func(r, bx) * t1;

                //if((ix*2 + nlfxdim) == 0 || (iy*2 + nlfydim) == 0 || (iz*2 + nlfzdim) == 0 ) 
                //    weptr[idx] = 0.0;

            }                   /* end for */

        }                       /* end for */

    }                           /* end for */

    fftw_execute_dft (p1, reinterpret_cast<fftw_complex*>(weptr), reinterpret_cast<fftw_complex*>(gwptr));

    pack_gftoc (sp, reinterpret_cast<fftw_complex*>(gwptr), reinterpret_cast<fftw_complex*>(rtptr));

    delete []gwptr;
    delete []weptr;
}

static double harmonic_func_00(double r, double *b)
{
    return sqrt(1.0/(4.0 * PI));
}

static double harmonic_func_10(double r, double *b)
{
    return sqrt(3.0/(4.0 * PI)) * b[0]/r;
}


static double harmonic_func_11(double r, double *b)
{
    return sqrt(3.0/(4.0 * PI)) * b[1]/r;
}

static double harmonic_func_12(double r, double *b)
{
    return sqrt(3.0/(4.0 * PI)) * b[2]/r;
}

static double harmonic_func_20(double r, double *b)
{
    return sqrt(15.0/(4.0 * PI)) * b[0] * b[1]/(r*r);
}

static double harmonic_func_21(double r, double *b)
{
    return sqrt(15.0/(4.0 * PI)) * b[0] * b[2]/(r*r);
}

static double harmonic_func_22(double r, double *b)
{
    return sqrt(5.0/(4.0 * PI))* (3.0 * b[2] * b[2] - r*r)/(2.0*r*r);
}

static double harmonic_func_23(double r, double *b)
{
    return sqrt(15.0/(4.0 * PI)) * b[1] * b[2]/(r*r);
}

static double harmonic_func_24(double r, double *b)
{
    return sqrt(15.0/(4.0 * PI))* (b[0] * b[0] - b[1]*b[1])/(2.0*r*r);
}


static double harmonic_func_30(double r, double *bx)
{
    double c0 = sqrt (35.0 / (2.0 * PI)) / 4.0;
    double rsq1 = r * r * r + 1.0e-20;
    return  c0 * (bx[1] * (3.0*bx[0]*bx[0] - bx[1]*bx[1])) / rsq1;
}

static double harmonic_func_31(double r, double *bx)
{
    double c1 = sqrt(105.0 / PI);
    double rsq1 = r * r * r + 1.0e-20;
    return  c1 * (bx[0] * bx[1] * bx[2]) / (2.0*rsq1);
}

static double harmonic_func_32(double r, double *bx)
{
    double c2 = sqrt(21.0 / (2.0 * PI)) / 4.0;
    double rsq1 = r * r * r + 1.0e-20;
    return  c2 * (bx[1] * (4.0*bx[2]*bx[2] - bx[0]*bx[0] - bx[1]*bx[1])) / rsq1;
}

static double harmonic_func_33(double r, double *bx)
{
    double c3 = sqrt(7.0 / PI) / 4.0;
    double rsq1 = r * r * r + 1.0e-20;
    return  c3 * (bx[2] * (2.0*bx[2]*bx[2] - 3.0*bx[0]*bx[0] - 3.0*bx[1]*bx[1])) / rsq1;
}

static double harmonic_func_34(double r, double *bx)
{
    double c2 = sqrt(21.0 / (2.0 * PI)) / 4.0;
    double rsq1 = r * r * r + 1.0e-20;
    return  c2 * (bx[0] * (4.0*bx[2]*bx[2] - bx[0]*bx[0] - bx[1]*bx[1])) / rsq1;
}

static double harmonic_func_35(double r, double *bx)
{
    double c1 = sqrt(105.0 / PI);
    double rsq1 = r * r * r + 1.0e-20;
    return  c1 * (bx[2] * (bx[0]*bx[0] - bx[1]*bx[1])) / (4.0*rsq1);
}

static double harmonic_func_36(double r, double *bx)
{
    double c0 = sqrt (35.0 / (2.0 * PI)) / 4.0;
    double rsq1 = r * r * r + 1.0e-20;
    return  c0 * (bx[0] * (bx[0]*bx[0] - 3.0*bx[1]*bx[1])) / rsq1;
}

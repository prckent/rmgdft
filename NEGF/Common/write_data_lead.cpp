#include "negf_prototypes.h"
/************************** SVN Revision Information **************************
 **    $Id$    **
 ******************************************************************************/

/*

   write_data.c


   Functions to write data to files.


 */



#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "main.h"
#include "init_var.h"
#include "LCR.h"
#include "pmo.h"





/* Writes the hartree potential, the wavefunctions, the */
/* compensating charges and various other things to a file. */
void write_data_lead (char *name, double *vh, double *vxc, double *vh_old, double *vxc_old,
		double *rho)
{
	int amode;
	char newname[MAX_PATH + 20];
	int fhand_rho, fhand_vxc, fhand_vh;
    FILE *fhand_EF;

	int size, rank, ndims, gsizes[2], distribs[2];
	int order,  dargs[2], psizes[2];
	MPI_Datatype mpi_darray_lead;
	int idx, i, iprobe;
	int mpi_amode = MPI_MODE_RDWR|MPI_MODE_CREATE;
	MPI_File mpi_fhand ;
	MPI_Info fileinfo;
	MPI_Status status;
	MPI_Offset disp, offset;


	/* Wait until everyone gets here */
	MPI_Barrier(pct.img_comm);

	/* Make the new output file name */

	if (pct.gridpe == 0)
	{
		sprintf (newname, "%s%s", name, ".vh");
		amode = S_IREAD | S_IWRITE;

		fhand_vh = open (newname, O_CREAT | O_TRUNC | O_RDWR, amode);
		if (fhand_vh < 0)
			error_handler (" Unable to write file for vh ");

		sprintf (newname, "%s%s", name, ".vxc");
		amode = S_IREAD | S_IWRITE;

		fhand_vxc = open (newname, O_CREAT | O_TRUNC | O_RDWR, amode);
		if (fhand_vxc < 0)
			error_handler (" Unable to write file for vxc ");

		sprintf (newname, "%s%s", name, ".rho");
		amode = S_IREAD | S_IWRITE;

		fhand_rho = open (newname, O_CREAT | O_TRUNC | O_RDWR, amode);
		if (fhand_rho < 0)
			error_handler (" Unable to write file for rho ");

		sprintf (newname, "%s%s", name, ".EF");
        fhand_EF = fopen (newname, "w");
        fprintf(fhand_EF, "%15.8f\n", lcr[1].EF_new);
        fclose(fhand_EF);


	}

	if (lcr[1].NY_GRID != get_NY_GRID())
		error_handler ("not a lead calculation y");
	if (lcr[1].NZ_GRID != get_NZ_GRID())
		error_handler ("not a lead calculation z");

	write_global_data_lead (fhand_vh, vh, lcr[1].NX_GRID * get_FG_RATIO() * 3, lcr[1].NY_GRID * get_FG_RATIO(),
			lcr[1].NZ_GRID * get_FG_RATIO());
	write_global_data_lead (fhand_vxc, vxc, lcr[1].NX_GRID * get_FG_RATIO() * 3, lcr[1].NY_GRID * get_FG_RATIO(),
			lcr[1].NZ_GRID * get_FG_RATIO());
	write_global_data_lead (fhand_rho, rho, lcr[1].NX_GRID * get_FG_RATIO() * 3, lcr[1].NY_GRID * get_FG_RATIO(),
			lcr[1].NZ_GRID * get_FG_RATIO());


	if (pct.gridpe == 0) 
    {
        close(fhand_vh);
        close(fhand_vxc);
        close(fhand_rho);
    }


	int ictxt = pmo.ictxt[pmo.myblacs];
	int mb = pmo.mblock;

	int nprow, npcol, myrow, mycol;

	Cblacs_gridinfo(ictxt, &nprow, &npcol, &myrow, &mycol);
	rank = myrow * pmo.ncol + mycol;

	size = pmo.nrow*pmo.ncol;
	ndims = 2;  
	distribs[0]= MPI_DISTRIBUTE_CYCLIC;
	distribs[1]= MPI_DISTRIBUTE_CYCLIC;
	dargs[0] = pmo.mblock;
	dargs[1] = pmo.mblock;
	psizes[0] = pmo.nrow;
	psizes[1] = pmo.ncol;
	order = MPI_ORDER_FORTRAN;

	gsizes[0] = lcr[1].num_states;
	gsizes[1] = lcr[1].num_states;

	MPI_Type_create_darray(size, rank, ndims, gsizes, distribs, 
			dargs, psizes, order, MPI_DOUBLE, &mpi_darray_lead);
	MPI_Type_commit(&mpi_darray_lead);


	MPI_Comm_rank(COMM_EN1, &rank);

	if(rank == 0)
	{

		/* write out the matrices for left lead */

		sprintf (newname, "%s%s", name, ".matrix");
		MPI_Info_create(&fileinfo);
		MPI_File_open(COMM_EN2, newname, mpi_amode, fileinfo, &mpi_fhand);

		disp=0;
		MPI_File_set_view(mpi_fhand, disp, MPI_DOUBLE, mpi_darray_lead, "native", MPI_INFO_NULL);
		idx = pmo.mxllda_lead[0]* pmo.mxlocc_lead[0];
		MPI_File_write_all(mpi_fhand, lcr[1].H00, idx, MPI_DOUBLE, &status);
		MPI_File_write_all(mpi_fhand, lcr[1].S00, idx, MPI_DOUBLE, &status);
		MPI_File_write_all(mpi_fhand, lcr[1].H01, idx, MPI_DOUBLE, &status);
		MPI_File_write_all(mpi_fhand, lcr[1].S01, idx, MPI_DOUBLE, &status);

		MPI_File_close(&mpi_fhand);
	}
}

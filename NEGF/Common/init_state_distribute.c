/************************** SVN Revision Information **************************
 **    $Id$    **
 ******************************************************************************/

/*

    read orbitals which has overlap with this processor domain and map it on
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


void init_state_distribute (STATE * states, STATE *states_distribute)
{
    int fhand;
    long nbytes;
    char newname[MAX_PATH + 200];
    char msg[200];

    int idx, idx0, idx2, subsystem;
    int ixmin, ixmax, iymin, iymax, izmin, izmax;
    int incx, incy, ix, iy, iz;
    double *array_tmp;

    int size, idx1;
    int NX, NY, NZ;
    int ixmin_old, ixmax_old, iymin_old, iymax_old, izmin_old, izmax_old;
    int st, st1, st2, st_new, max_orbit_nx_ny;
    int item;

    int x_off, y_off, z_off;
    int istart, block_i, st_in_block;

    rmg_double_t *psi_old, *psi_new, *psi_whole; 
    rmg_double_t hx_old, hx_new, hy_old, hy_new;
    rmg_double_t x1_old, x1_new, y1_old, y1_new;

    max_orbit_nx_ny = max(ct.max_orbit_nx, ct.max_orbit_ny);
    my_malloc_init( psi_old, max_orbit_nx_ny, rmg_double_t );
    my_malloc_init( psi_new, max_orbit_nx_ny, rmg_double_t );

    size = ct.max_orbit_nx * ct.max_orbit_ny * ct.max_orbit_nz;
    my_malloc_init( psi_whole, size, rmg_double_t );
    my_malloc_init( array_tmp, size, rmg_double_t );

    hx_new = get_hxgrid() * get_xside();
    hy_new = get_hygrid() * get_yside();

    /* Wait until everybody gets here */
    my_barrier ();

    /*
       printf ("state_begin, state_end %d %d \n", ct.state_begin, ct.state_end);
       */


    x_off = get_PX_OFFSET();
    y_off = get_PY_OFFSET();
    z_off = get_PZ_OFFSET();

//  local_index = -1 means that this orbital is not on this process
    for (st = 0; st < ct.num_states; st++) states_distribute[st].local_index = -1;

    pct.num_local_orbit = 0;
    istart = 0;
    for(block_i = 0; block_i < ct.num_blocks; block_i++)
    {
        for(st_in_block = 0; st_in_block < ct.block_dim[block_i]; st_in_block++)
        {

            st = istart + st_in_block;

            if(states[st].ixmax < x_off || states[st].ixmin > x_off + get_PX0_GRID()) continue;
            if(cei.num_probe >2)    
            {
                if(states[st].iymax <y_off || states[st].iymin > y_off + get_PY0_GRID()) continue;
            }

            iymin = states[st].iymin;
            iymax = states[st].iymax;
            if(iymin >= 0 && iymax < get_NY_GRID())
            {
                if(states[st].iymax <y_off || states[st].iymin > y_off + get_PY0_GRID()) continue;
            }

            if(iymin < 0)
            {
                iymin += get_NY_GRID();
                if(y_off > states[st].iymax && y_off + get_PY0_GRID() < iymin) continue;
            }

            if(iymax >= get_NY_GRID() ) 
            {
                iymax -= get_NY_GRID();
                if(y_off > iymax && y_off + get_PY0_GRID() < iymin) continue;
            }

            izmin = states[st].izmin;
            izmax = states[st].izmax;
            if(izmin >= 0 && izmax < get_NZ_GRID())
            {
                if(states[st].izmax <z_off || states[st].izmin > z_off + get_PZ0_GRID()) continue;
            }

            if(izmin < 0)
            {
                izmin += get_NZ_GRID();
                if(z_off > states[st].izmax && z_off + get_PZ0_GRID() < izmin) continue;
            }

            if(izmax >= get_NZ_GRID() ) 
            {
                izmax -= get_NZ_GRID();
                if(z_off > izmax && z_off + get_PZ0_GRID() < izmin) continue;
            }


            states_distribute[pct.num_local_orbit].istate = st;
            states_distribute[st].local_index = pct.num_local_orbit;
            states_distribute[pct.num_local_orbit].whichblock = block_i;
            states_distribute[pct.num_local_orbit].istate_in_block = st_in_block;
            pct.num_local_orbit++;
        }

        istart += ct.block_dim[block_i];
    }


    dprintf("\n pct.num_local_orbit = %d", pct.num_local_orbit);
    double *rptr;
    size = pct.num_local_orbit * get_P0_BASIS()+1024;
    printf("\n pct.num_local_orbit = %d size = %d", pct.num_local_orbit, size);
    my_malloc_init( rptr, size, rmg_double_t );
    my_malloc_init( work_memory, size, rmg_double_t );

    for (st1 = 0; st1 < pct.num_local_orbit; st1++)
    {
        states_distribute[st1].psiR = rptr;
        rptr += get_P0_BASIS();
    }




    for (st2 = 0; st2 < pct.num_local_orbit; st2++)
    {
        st = states_distribute[st2].istate;
        x1_new = states[st].ixmin * hx_new;
        y1_new = states[st].iymin * hy_new;
        NX = states[st].orbit_nx;
        NY = states[st].orbit_ny;
        NZ = states[st].orbit_nz;
        incx = NY * NZ;
        incy = NZ * NX;
        st_new = 0;
        for (subsystem = 0; subsystem < cei.num_subsystem; subsystem++)
        {
            idx0 = cei.subsystem_idx[subsystem];


            for (st1 = lcr[idx0].state_begin; st1 < lcr[idx0].state_end; st1++)
            {
                if(st_new == st)
                {
                    sprintf (newname, "%s%s%d", lcr[idx0].name, ".orbit_", st1);
                    my_open( fhand, newname, O_RDWR, S_IREAD | S_IWRITE );
                    if (fhand < 0)
                    {
                        printf ("\n %s, st1 = %d %d", newname, st1, st);
                        error_handler (" Unable to open file ");
                    }

                    idx = states[st].size * (int) sizeof (double);

                    /* ====================== Reading orbitals ======================= */                    

                    /*
                       printf ("reading orbitals for 3rd/4th probe, idx0 =  %d %s \n", idx0, newname);
                     */

                    nbytes = read (fhand, array_tmp, idx);
                    nbytes = read (fhand, &ixmin_old, sizeof (int));
                    nbytes = read (fhand, &ixmax_old, sizeof (int));
                    nbytes = read (fhand, &iymin_old, sizeof (int));
                    nbytes = read (fhand, &iymax_old, sizeof (int));
                    if (nbytes != sizeof (int))
                    {
                        printf ("\n read %d is different from %d for state %d", (int) nbytes,
                                (int) sizeof (int), st);
                        error_handler ("Unexpected end of file orbit");
                    }

                    if(idx0 <= 2 | idx0 > cei.num_probe) /* Satisfies left probe, central parta & right probe */
                    {
                        for(ix = 0; ix < states[st].size; ix++)
                            psi_whole[ix] = array_tmp[ix];

                    }
                    else /* Satisfies down and up probes */ 
                    {

                        item = ixmin_old;
                        ixmin_old = iymin_old;
                        iymin_old = item;
                        incx = states[st].orbit_nz * states[st].orbit_ny;
                        incy = states[st].orbit_nz * states[st].orbit_nx;
                        for(ix = 0; ix < states[st].orbit_nx; ix++)
                        {
                            for(iy = 0; iy < states[st].orbit_ny; iy++)
                            {
                                for(iz = 0; iz < states[st].orbit_nz; iz++)
                                {
                                    idx = iz + iy * states[st].orbit_nz + ix * incx;
                                    idx2= iz + ix * states[st].orbit_nz + iy * incy; /* Check */ 
                                    psi_whole[idx] = array_tmp[idx2];
                                }
                            }
                        }

                    }



                    close (fhand);

                    hx_old = lcr[idx0].xside / lcr[idx0].NX_GRID;
                    x1_old = lcr[idx0].x_shift + ixmin_old  * hx_old;

                    /* ==== Interpolation along x-direction ==== */
                    for (iy = 0; iy < NY; iy++)
                    {
                        for (iz = 0; iz < NZ; iz++)
                        {
                            for (ix = 0; ix < NX; ix++)
                            {
                                idx1 = iz + iy * NZ + ix * incx;
                                psi_old[ix] = psi_whole[idx1];
                            }
                            diff_hx_interpolation (st, psi_new, psi_old, NX, hx_new, hx_old, x1_new, x1_old);

                            for (ix = 0; ix < NX; ix++)
                            {
                                idx1 = iz + iy * NZ + ix * incx;
                                psi_whole[idx1] = psi_new[ix];
                            }
                        }
                    }

                    hy_old = lcr[idx0].yside / lcr[idx0].NY_GRID;
                    y1_old = lcr[idx0].y_shift + iymin_old  * hy_old;
                    /*
                       y1_old = lcr[idx0].y_shift + iymin_old * hy_old;
                       y1_new = (iymin_new + lcr[idx0].y0) * hy_new;
                     */

                    /* ==== Interpolation along y-direction ==== */

                    for (iz = 0; iz < NZ; iz++)
                    {
                        for (ix = 0; ix < NX; ix++)
                        {
                            for (iy = 0; iy < NY; iy++)
                            {
                                idx1 = iz + iy * NZ + ix * incx;
                                psi_old[iy] = psi_whole[idx1];
                            }

                            diff_hx_interpolation (-st, psi_new, psi_old, NY, hy_new, hy_old, y1_new, y1_old); 

                            for (iy = 0; iy < NY; iy++)
                            {
                                idx1 = iz + iy * NZ + ix * incx;
                                psi_whole[idx1] = psi_new[iy];
                            }
                        }
                    }

                }   /* if statement ends */

                st_new++;

            }   /* loop st1 */
        }   /* subsystem loop ends */

        map_orbital_to_process(st2, states, states_distribute, psi_whole);
    }   
}   



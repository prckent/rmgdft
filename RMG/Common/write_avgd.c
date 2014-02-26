/************************** SVN Revision Information **************************
 **    $Id$    **
******************************************************************************/

/****f* QMD-MGDFT/write_avgd.c *****
 * NAME
 *   Ab initio real space code with multigrid acceleration
 *   Quantum molecular dynamics package.
 *   Version: 2.1.5
 * COPYRIGHT
 *   Copyright (C) 1995  Emil Briggs
 *   Copyright (C) 1998  Emil Briggs, Charles Brabec, Mark Wensell, 
 *                       Dan Sullivan, Chris Rapcewicz, Jerzy Bernholc
 *   Copyright (C) 2001  Emil Briggs, Wenchang Lu,
 *                       Marco Buongiorno Nardelli,Charles Brabec, 
 *                       Mark Wensell,Dan Sullivan, Chris Rapcewicz,
 *                       Jerzy Bernholc
 * FUNCTION
 *   void write_avgd(rmg_double_t *rho)
 *   Generates the average electron density along the z-axis.
 *   Integrates over the x and y axes.
 * INPUTS
 *   rho: total valence charge density
 * OUTPUT
 *   the average over xy plane is printed out
 * PARENTS
 *   main.c
 * CHILDREN
 *   globlal_sums.c pe2xyz.c
 * SOURCE
 */


#include <stdio.h>
#include "grid.h"
#include "common_prototypes.h"
#include "main.h"

void write_avgd (rmg_double_t * rho)
{

    int ix, iy, iz, poff;
    int px, py, pz;
    int FPX0_GRID, FPY0_GRID, FPZ0_GRID;
    int FNZ_GRID;

    rmg_double_t t1;
    rmg_double_t *zvec;
    rmg_double_t hzzgrid;

    hzzgrid = get_hzzgrid();

    FPX0_GRID = get_FPX0_GRID();
    FPY0_GRID = get_FPY0_GRID();
    FPZ0_GRID = get_FPZ0_GRID();
    FNZ_GRID = get_FNZ_GRID();

    my_malloc(zvec, FNZ_GRID, rmg_double_t);

    /* Get this processors offset */
    pe2xyz (pct.gridpe, &px, &py, &pz);
    poff = pz * FPZ0_GRID;


    /* Zero out result vector */
    for (iz = 0; iz < FNZ_GRID; iz++)
        zvec[iz] = ZERO;


    /* Loop over this processor */
    for (iz = 0; iz < FPZ0_GRID; iz++)
    {

        t1 = ZERO;
        for (ix = 0; ix < FPX0_GRID; ix++)
        {

            for (iy = 0; iy < FPY0_GRID; iy++)
            {

                t1 += rho[ix * FPY0_GRID * FPZ0_GRID + iy * FPZ0_GRID + iz];

            }                   /* end for */

        }                       /* end for */

        t1 = t1 * get_vel() / hzzgrid;

        zvec[iz + poff] = t1;

    }                           /* end for */


    /* Now sum over all processors */
    iz = FNZ_GRID;
    global_sums (zvec, &iz, pct.grid_comm);

    if (pct.gridpe == 0)
    {
        printf ("\n\n Planar average of the electrostatic density\n");
        for (iz = 0; iz < FNZ_GRID; iz++)
        {
            t1 = iz * hzzgrid;
            printf (" %f %f\n", t1, zvec[iz]);
        }
        fflush (NULL);
    }

    free(zvec);
}                               /* end get_avgd */

/******/

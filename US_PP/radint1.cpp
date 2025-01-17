/****f* QMD-MGDFT/radint1.c *****
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
 *   double radint1( double *f, double *r, double *dr_di, int n )
 *   Radial integration function for data defined on a almost-log grid.
 *   The Simpson's rule is used.
 *   We assume that the function being integrated goes to zero 
 *   for large r-values.
 * INPUTS
 *   f:       function being integrated
 *   r:       array of r-values in mesh
 *   dr_di:   derivative of 'r' relative to 'i', the mesh index
 *   n:       number of mesh points
 * OUTPUT
 *   integrated result is returned
 * PARENTS
 *   init_kbr.c rft.c
 * CHILDREN
 *   nothing 
 * SOURCE
 */
#include <math.h>
#include <float.h>
#include "main.h"


/* 
 * Updated: Filipe Ribeiro Jul 2006
 * This is an implementation of Simpson's rule for numerical integration.
 * Because the Simpson's rule only works for uniform grids, one needs to 
 * change variables from 'dr' to 'di' and therefore the 'dr_di' must be provided.
 * Previously, 'dr_di' was confusingly named 'rab'. 
 * It is assumed that 'f' goes to zero for large 'r' faster than 'r^2'.
 * Updated by Emil Briggs for performance and compatibility reasons Jul 2019.
 * While not physically meaningful small changes in the integration routine
 * can cause discrepancies when comparing with other codes.
 */

double radint1 (double * f, double * r, double * dr_di, int n)
{

    /* Simpson's rule weights */
    const double w[2] = {2.0 / 3.0, 4.0 / 3.0};
    const unsigned mask = {1};

    double w0 = 1.0 / 3.0;
    double sum = w0 * f[0] * r[0] * r[0] * dr_di[0];    /* this is 0 because r[0] = 0 */

    for (unsigned i = 1; i < n; i++)
        sum += f[i] * r[i] * r[i] * dr_di[i] * w[i & mask];

    return sum;
}



/******/

/*
 *
 * Copyright 2014 The RMG Project Developers. See the COPYRIGHT file 
 * at the top-level directory of this distribution or in the current
 * directory.
 * 
 * This file is part of RMG. 
 * RMG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * any later version.
 *
 * RMG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/


#ifndef RMG_Functional_H
#define RMG_Functional_H 1

#include <string>
#include "BaseGrid.h"
#include "Lattice.h"
#include "TradeImages.h"
#include "vdW.h"
#include "xc.h"

class Functional {

private:
    // BaseGrid class
    BaseGrid *Grid;

    // TradeImages object to use
    TradeImages *T;

    // Lattice object
    Lattice *L;

    int pbasis;     // Grid points on this processing node
    double hxgrid;
    double hygrid;
    double hzgrid;
    int dimx;
    int dimy;
    int dimz;

    // Total number of grid points
    int N;



    //
    std::string dft_name;

    static int iexch;           // type of exchange
    static int icorr;           // type of correlation
    static int igcx;            // type of gradient correction on exchange
    static int igcc;            // type of gradient correction on correlation
    static int inlc;            // type of non local correction on correlation
    static int imeta;           // type of meta-GGA

    void gradcorr(double *rho, double *rho_core, double &etxc, double &vtxc, double *v);

public:
    Functional (BaseGrid &G, 
                Lattice &L, 
                TradeImages &T, 
                bool gamma_flag);

    ~Functional(void);

    void set_dft_from_name(char *newdft_name);
    void set_dft_from_name(std::string newdft_name);
    bool dft_is_gradient(void);
    bool dft_is_meta(void);
    bool dft_is_hybrid(void);
    bool igcc_is_lyp(void);
    bool dft_is_nonlocc(void);
    bool dft_has_finite_size_correction(void);
    void v_xc(double *rho, double *rho_core, double &etxc, double &vtxc, double *v, int spinflag );

};

#endif


#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "const.h"
#include "RmgTimer.h"
#include "RmgException.h"
#include "rmgtypedefs.h"
#include "rmg_error.h"
#include "transition.h"

#include "LaplacianCoeff.h"



void SetLaplacian()
{

    double a[3][3];
    int Ngrid[3], dim[3];
    int Lorder;
    Lorder = ct.kohn_sham_fd_order;
    for(int i = 0; i < 3; i++)
    {
        a[0][i] = Rmg_L.a0[i];
        a[1][i] = Rmg_L.a1[i];
        a[2][i] = Rmg_L.a2[i];
    }

    Ngrid[0] = Rmg_G->get_NX_GRID(1);
    Ngrid[1] = Rmg_G->get_NY_GRID(1);
    Ngrid[2] = Rmg_G->get_NZ_GRID(1);
    dim[0] = Rmg_G->get_PX0_GRID(1);
    dim[1] = Rmg_G->get_PY0_GRID(1);
    dim[2] = Rmg_G->get_PZ0_GRID(1);

    LC = new LaplacianCoeff(a, Ngrid, Lorder, dim);

    LC->SetOffdiag(ct.laplacian_offdiag);

    LC->CalculateCoeff();

}


/*
 *
 * Copyright 2018 The RMG Project Developers. See the COPYRIGHT file 
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


#include "GpuAlloc.h"
#include "rmg_error.h"
#include "transition.h"
#include "ErrorFuncs.h"
#include "Gpufuncs.h"
#include "RmgMatrix.h"
#include "blas.h"

#if GPU_ENABLED
#include <cuda_runtime.h>
#include <cusolverDn.h>

void DsyevdDriver(double *A, double *eigs, double *work, int worksize, int n)
{

    cusolverStatus_t cu_status;
    int lwork, *devInfo;
    const cusolverEigMode_t jobz = CUSOLVER_EIG_MODE_VECTOR; // compute eigenvectors.
    const cublasFillMode_t  uplo = CUBLAS_FILL_MODE_LOWER;


    cu_status = cusolverDnDsyevd_bufferSize(ct.cusolver_handle, jobz, uplo, n, A, n, eigs, &lwork);
    if(cu_status != CUSOLVER_STATUS_SUCCESS) rmg_error_handler (__FILE__, __LINE__, " cusolverDnDsyevd_bufferSize failed.");
    if(lwork > worksize) rmg_error_handler (__FILE__, __LINE__, " DsyevdDriver: provided workspace too small.");

    RmgCudaError(__FILE__, __LINE__, cudaMalloc((void **)&devInfo, sizeof(int) ), "Problem with cudaMalloc");

    cu_status = cusolverDnDsyevd(ct.cusolver_handle, jobz, uplo, n, A, n, eigs, work, lwork, devInfo);
    int info;
    cudaMemcpy(&info, devInfo, sizeof(int), cudaMemcpyDeviceToHost);
    if(cu_status != CUSOLVER_STATUS_SUCCESS) rmg_error_handler (__FILE__, __LINE__, " cusolverDnDsyevd failed.");

    cudaFree(devInfo);
}

#else

void DsyevdDriver(double *A, double *eigs, double *work, int worksize, int n)
{
    char *cuplo = "l", *jobz="V";
    int lwork, info=0, *iwork, liwork;

    liwork = 6*n;
    iwork = new int[liwork];

    lwork = worksize;

    dsyevd(jobz, cuplo, &n, A, &n, eigs, work, &lwork, iwork, &liwork, &info);

    if(info)
        rmg_error_handler (__FILE__, __LINE__, " dsyevd failed.");

    delete [] iwork;
}
#endif

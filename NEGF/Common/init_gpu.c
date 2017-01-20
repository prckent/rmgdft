/*   init_gpu.c
 * COPYRIGHT
 *   Copyright (C) 1995  Emil Briggs
 *   Copyright (C) 1998  Emil Briggs, Charles Brabec, Mark Wensell, 
 *                       Dan Sullivan, Chris Rapcewicz, Jerzy Bernholc
 *   Copyright (C) 2001  Emil Briggs, Wenchang Lu,
 *                       Marco Buongiorno Nardelli,Charles Brabec, 
 *                       Mark Wensell,Dan Sullivan, Chris Rapcewicz,
 *                       Jerzy Bernholc
 * FUNCTION
 *   void init_gpu(void)
 *   sets up gpu
 * INPUTS
 *   nothing
 * OUTPUT
 *   nothing
 * PARENTS
 *   init.c
 * CHILDREN
 *   nothing
 * SOURCE
 */



#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "init_var.h"
#include "LCR.h"
#include "pmo.h"

#if GPU_ENABLED

void rmg_printout_devices( )
{
	int ndevices, idevice;
	cuDeviceGetCount( &ndevices );
	char name[200];
#if CUDA_VERSION > 3010 
	size_t totalMem;
#else
	unsigned int totalMem;
#endif

	int clock;
	CUdevice dev;

	for(idevice = 0; idevice < ndevices; idevice++ ) {

		cuDeviceGet( &dev, idevice );
		cuDeviceGetName( name, sizeof(name), dev );
		cuDeviceTotalMem( &totalMem, dev );
		cuDeviceGetAttribute( &clock,
				CU_DEVICE_ATTRIBUTE_CLOCK_RATE, dev );
		if(idevice < 10) printf( "device %d: %s, %.1f MHz clock, %.1f MB memory\n",
				idevice, name, clock/1000.f, totalMem/1024.f/1024.f );
	}
}



cudaStream_t rmg_default_cuda_stream;
void init_gpu (void)
{

	cublasStatus_t custat;
	int alloc;
	int i, ntot_row, maxrow;

	rmg_printout_devices( );


	alloc = pmo.ntot_low * sizeof(complex double);
	if( cudaSuccess != cudaMalloc((void **)&ct.gpu_Htri , alloc )){
		fprintf (stderr, "!!!! cublasAlloc failed for: gpu_GdiagBlocks %d\n", alloc);
		exit(-1);
	}
	if( cudaSuccess != cudaMalloc((void **)&ct.gpu_Gtri , alloc )){
		fprintf (stderr, "!!!! cublasAlloc failed for: gpu_GdiagBlocks %d\n", alloc);
		exit(-1);
	}

    int size;
    size = 0;
	for(i = 0; i < ct.num_blocks; i++)
	{
        size += ct.block_dim[i]*ct.block_dim[i];
    }
	alloc = size * sizeof(complex double);
	if( cudaSuccess != cudaMalloc((void **)&ct.gpu_GdiagBlocks , alloc )){
		fprintf (stderr, "!!!! cublasAlloc failed for: gpu_GdiagBlocks %d\n", alloc);
        
		exit(-1);
	}


	ntot_row = 0;
	maxrow = 0;
	for(i = 0; i < ct.num_blocks; i++)
	{
		ntot_row += ct.block_dim[i];
		maxrow = rmg_max(maxrow, ct.block_dim[i]);
	}

	alloc = ntot_row * maxrow * sizeof(complex double);
	if( cudaSuccess != cudaMalloc((void **)&ct.gpu_Grow , alloc ) ){
		fprintf (stderr, "Error: cudaMalloc failed for: gpu_Grow\n");
		exit(-1);
	}
	if( cudaSuccess != cudaMalloc((void **)&ct.gpu_Gcol , alloc ) ){
		fprintf (stderr, "Error: cudaMalloc failed for: gpu_Grow\n");
		exit(-1);
	}

	alloc = maxrow * maxrow * sizeof(complex double);
	if( cudaSuccess != cudaMalloc((void **)&ct.gpu_Hii, alloc )){
		fprintf (stderr, "Error: cudaMalloc failed for: ct.gpu_Hii\n");
		exit(-1);
	}
	if( cudaSuccess != cudaMalloc((void **)&ct.gpu_Gii, alloc )){
		fprintf (stderr, "Error: cudaMalloc failed for: ct.gpu_Gii\n");
		exit(-1);
	}
	if( cudaSuccess != cudaMalloc((void **)&ct.gpu_temp, alloc )){
		fprintf (stderr, "Error: cudaMalloc failed for: ct.gpu_temp\n");
		exit(-1);
	}
	if( cudaSuccess != cudaMalloc((void **)&ct.gpu_Imatrix, alloc )){
		fprintf (stderr, "Error: cudaMalloc failed for: ct.gpu_Imatrix\n");
		exit(-1);
	}

	alloc = maxrow * sizeof(int);
	if( cudaSuccess != cudaMalloc((void **)&ct.gpu_ipiv, alloc )){
		fprintf (stderr, "Error: cudaMalloc failed for: ct.gpu_ipiv\n");
		exit(-1);
	}


	custat = cublasCreate(&ct.cublas_handle);
	if( custat != CUBLAS_STATUS_SUCCESS ) {
		fprintf (stderr, "Error cublasCreate failed for: ct.cublas_handle\n");
		exit(-1);
	}

	cudaSetDeviceFlags(cudaDeviceScheduleSpin);
	// Create a stream for the main process
	cudaStreamCreate(&ct.cuda_stream);
	//  cublasSetStream(ct.cublas_handle, ct.cuda_stream); 
	//  magmablasSetKernelStream(ct.cuda_stream);

}

void finalize_gpu (void)
{

	cublasDestroy(ct.cublas_handle);
//	cudaFree(ct.gpu_global_matrix);
	cudaFree(ct.gpu_Grow);
	cudaFree(ct.gpu_Gcol);
//	cudaFree(ct.gpu_work1);
//	cudaFree(ct.gpu_work2);
//	cudaFree(ct.gpu_work3);
//	cudaFree(ct.gpu_work4);

	//    cuCtxDetach( ct.cu_context ); 
	//   cublasShutdown();

}

#endif

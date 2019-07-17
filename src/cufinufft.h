#ifndef __CUFINUFFT_H__
#define __CUFINUFFT_H__

#include <cufft.h>
#include <cstdlib>
#include <assert.h>
#include "../finufft/utils.h"

#define MAX_NSPREAD 16
#define RESCALE(x,N,p) (p ? \
		((x*M_1_2PI + (x<-PI ? 1.5 : (x>PI ? -0.5 : 0.5)))*N) : \
		(x<0 ? x+N : (x>N ? x-N : x)))

struct cufinufft_opts {     // see cuspread:setup_spreader for defaults.
	int nspread;            // w, the kernel width in grid pts
	int spread_direction;   // 1 means spread NU->U, 2 means interpolate U->NU
	int pirange;            // 0: coords in [0,N), 1 coords in [-pi,pi)
	FLT upsampfac;          // sigma, upsampling factor, default 2.0

	// ES kernel specific...
	FLT ES_beta;
	FLT ES_halfwidth;
	FLT ES_c;

	// CUDA, variables
	int method;
	int bin_size_x;
	int bin_size_y;
	int Horner;
	int maxsubprobsize;
	int nthread_x; //now threads block size is hard coded, not used
	int nthread_y; 
	int nstreams; 
};

struct cufinufft_plan {
	int M;
	int nf1;
	int nf2;
	int ms;
	int mt;
	int ntransf;
	int ntransfcufftplan;
	int iflag;

	int totalnumsubprob;
	int byte_now;
	FLT *fwkerhalf1;
	FLT *fwkerhalf2;

	FLT *kx;
	FLT *ky;
	CUCPX *c;
	CUCPX *fw;
	CUCPX *fk;

	// Arrays that used in subprob method
	int *idxnupts;
	int *sortidx;
	int *numsubprob;
	int *binsize;
	int *binstartpts;
	int *subprob_to_bin;
	int *subprobstartpts;

	// Extra arrays for Paul's method
	int *finegridsize;
	int *fgstartpts;

	cufftHandle fftplan;
	cudaStream_t *streams;
};

// For error checking (where should this function be??)
static const char* _cufftGetErrorEnum(cufftResult_t error)
{
	switch(error)
	{
		case CUFFT_SUCCESS:
			return "cufft_success";
		case CUFFT_INVALID_PLAN:
			return "cufft_invalid_plan";
		case CUFFT_ALLOC_FAILED:
			return "cufft_alloc_failed";
		case CUFFT_INVALID_TYPE:
			return "cufft_invalid_type";
		case CUFFT_INVALID_VALUE:
			return "cufft_invalid_value";
		case CUFFT_INTERNAL_ERROR:
			return "cufft_internal_error";
		case CUFFT_EXEC_FAILED:
			return "cufft_exec_failed";
		case CUFFT_SETUP_FAILED:
			return "cufft_setup_failed";
		case CUFFT_INVALID_SIZE:
			return "cufft_invalid_size";
		case CUFFT_UNALIGNED_DATA:
			return "cufft_unaligned data";
		case CUFFT_INCOMPLETE_PARAMETER_LIST:
			return "cufft_incomplete_parameter_list";
		case CUFFT_INVALID_DEVICE:
			return "cufft_invalid_device";
		case CUFFT_PARSE_ERROR:
			return "cufft_parse_error";
		case CUFFT_NO_WORKSPACE:
			return "cufft_no_workspace";
		case CUFFT_NOT_IMPLEMENTED:
			return "cufft_not_implemented";
		case CUFFT_LICENSE_ERROR:
			return "cufft_license_error";
		case CUFFT_NOT_SUPPORTED:
			return "cufft_not_supported";
	}
	return "<unknown>";
}
#define checkCufftErrors(call)
int cufinufft_default_opts(cufinufft_opts &opts,FLT eps,FLT upsampfac);

// 2d
int cufinufft2d_plan(int M, int ms, int mt, int ntransf, int ntransfcufftplan, 
		int iflag, const cufinufft_opts opts, 
		cufinufft_plan *d_plan);
int cufinufft2d_setNUpts(FLT* h_kx, FLT* h_ky, cufinufft_opts &opts, cufinufft_plan *d_plan);
int cufinufft2d1_exec(CPX* h_c, CPX* h_fk, cufinufft_opts &opts, cufinufft_plan *d_plan);
int cufinufft2d2_exec(CPX* h_c, CPX* h_fk, cufinufft_opts &opts, cufinufft_plan *d_plan);
int cufinufft2d_destroy(const cufinufft_opts opts, cufinufft_plan *d_plan);
#endif

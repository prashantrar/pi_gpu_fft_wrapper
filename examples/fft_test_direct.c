#include <stdio.h>
#include <stdlib.h>
#include "mailbox.h"
#include "gpu_fft.h"
#include "papi.h"
#define log2_N 8

int main()
{

	int i, j, k, ret, jobs, N, mb = mbox_open();
	long long ptimer1 = 0;
	long long ptimer2 = 0;
	struct GPU_FFT_COMPLEX *base;
    	struct GPU_FFT *fft;
	N = 1<<log2_N; // FFT length
	jobs = 1;
    	ret = gpu_fft_prepare(mb, log2_N, GPU_FFT_FWD, jobs, &fft); // call once
	for (j=0; j<jobs; j++) {
		base = fft->in + j*fft->step; // input buffer
		for (i=0; i<N; i++){ base[i].re = 0.5; base[i].im = 0;}//giving input to fft
	}
	usleep(1); // Yield to OS
	ptimer1 = PAPI_get_real_usec();
	gpu_fft_execute(fft); // call one or many times
	ptimer2 = PAPI_get_real_usec();
	printf("Time elapsed is (PAPI)%llu\n",(ptimer2-ptimer1));
#ifdef TEST
	for (j=0; j<jobs; j++) {
		base = fft->out + j*fft->step; // output buffer
		for (i=0; i<N; i++) {
			printf("fft real[%f] imag[%f]\n",base[i].re,base[i].im);		
		}
	}
#endif
	gpu_fft_release(fft); // Videocore memory lost if not freed !
	return 0;

}




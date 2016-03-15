/*
   BCM2835 "GPU_FFT" release 3.0
   Copyright (c) 2015, Andrew Holme.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the copyright holder nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include "mailbox.h"
#include "gpu_fft.h"

char Usage[] =
"Usage: hello_fft.bin log2_N [jobs [loops]]\n"
"log2_N = log2(FFT_length),       log2_N = 8...22\n"
"jobs   = transforms per batch,   jobs>0,        default 1\n"
"loops  = number of test repeats, loops>0,       default 1\n";

int perf_gpu_fft(int log2_N, int jobs, float *real, float *imag) {
	int i, j, ret, N, mb = mbox_open();
	struct GPU_FFT_COMPLEX *base;
	struct GPU_FFT *fft;
	log2_N = log2_N?log2_N:8; // 8 <= log2_N <= 22
	jobs   = jobs?jobs:1;  // transforms per batch
	if (log2_N<2 || jobs<1) {
		printf(Usage);
		return -1;
	}
	N = 1<<log2_N; // FFT length
	ret = gpu_fft_prepare(mb, log2_N, GPU_FFT_REV, jobs, &fft); // call once
	switch(ret) {
		case -1: printf("Unable to enable V3D. Please check your firmware is up to date.\n"); return -1;
		case -2: printf("log2_N=%d not supported.  Try between 8 and 22.\n", log2_N);         return -1;
		case -3: printf("Out of memory.  Try a smaller batch or increase GPU memory.\n");     return -1;
		case -4: printf("Unable to map Videocore peripherals into ARM memory space.\n");      return -1;
		case -5: printf("Can't open libbcm_host.\n");                                         return -1;
	}
	for (j=0; j<jobs; j++) {
		base = fft->in + j*fft->step; // input buffer
		for (i=0; i<N; i++){ base[i].re = real[i] ; base[i].im = imag[i];}
	}
	usleep(1); // Yield to OS
	gpu_fft_execute(fft); // call one or many times
	for (j=0; j<jobs; j++) {
		base = fft->out + j*fft->step; // output buffer
		for (i=0; i<N; i++) {
			real[i] = base[i].re; imag[i] = base[i].im;		
		}
	}
	gpu_fft_release(fft); // Videocore memory lost if not freed !
	return 0;
}

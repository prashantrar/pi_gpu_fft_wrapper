/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include "papi.h"


#include <math.h>

//**************fft****************//
#include "kiss_fftr.h"
#define N 16384
#define FIXED_POINT


int main()
{
	register int j,k;
	long long ptimer1 = 0;
	long long ptimer2 = 0;
	kiss_fft_scalar in[N];
	kiss_fft_cpx out[N / 2 + 1];
	for (j = 0; j < N; j++){
		in[j] = 0.5;
	}
	kiss_fftr_cfg cfg;

	if ((cfg = kiss_fftr_alloc(N, 0/*is_inverse_fft*/, NULL, NULL)) != NULL)
	{
		ptimer1 = PAPI_get_real_usec();
		kiss_fftr(cfg, in, out);
		ptimer2 = PAPI_get_real_usec();
		printf("Time elapsed is (PAPI)%llu\n",(ptimer2-ptimer1));
		free(cfg);

		for(k=0;k<=N/2;k++)
		{
			printf("real[%d] = %f, imag[%d] = %f\n",k,out[k].r,k,out[k].i);
		}
	}
	else
	{
		printf("err\n");
	}

	return 0;
}



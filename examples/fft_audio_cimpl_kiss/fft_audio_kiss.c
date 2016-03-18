
#include "wavfile.h"
#include <sndfile.h>
#include <malloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "papi.h"

#include <math.h>
#include "kiss_fftr.h"
#define N 16384
#define FIXED_POINT
#define USE_SIMD

int main()
{
	long long ptimer1 = 0;
	long long ptimer2 = 0;
	SNDFILE *sf;
	SF_INFO info;
	int num_channels;
	int num, num_items;
	//short *buf;
	float *buf;
	int f, sr, c;
	volatile register int i, j, k;
	FILE *fileout;
	FILE *out_file;
	
	kiss_fft_scalar in[N];
	kiss_fft_cpx out[N];
	kiss_fftr_cfg cfg;

	printf("size of short = %d\n", sizeof(short));

	/* Open the WAV file. */
	info.format = 0;
	sf = sf_open("120615_f0012_m1307.52.wav", SFM_READ, &info);
	if (sf == NULL)
	{
		printf("Failed to open the file.\n");
		exit(-1);
	}

	/* Print some of the info, and figure out how much data to read. */
	f = info.frames;
	sr = info.samplerate;
	c = info.channels;
	num_items = f*c;

	printf("frames=%d\n", f);
	printf("samplerate=%d\n", sr);
	printf("channels=%d\n", c);
	printf("num_items=%d\n", num_items);

	// Check format - 16bit PCM
	if (info.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
		fprintf(stderr, "Input should be 16bit Wav\n");
		sf_close(sf);
		return 1;
	}

	// Check channels - mono
	if (info.channels != 1) {
		fprintf(stderr, "Wrong number of channels\n");
		sf_close(sf);
		return 1;
	}

	/* Allocate space for the data to be read, then read it. */
	buf = (float*)malloc(num_items*sizeof(float));
	num = sf_read_float(sf, buf, num_items);
	sf_close(sf);
	printf("Read %d items(num)\n", num);
	printf("size of buf = %d\n", sizeof(buf));
	
	
	for (i = 0; i < num_items; i++)
	{
		printf("buf[%x] = %f\n", i, buf[i]);
	}
	

	/* Write the data to filedata.out. */
	/*out = fopen("filedata.out", "wb");
	fwrite(buf, sizeof(short), num_items, fileout);
	fclose(out);

	*Creating new .wav file --- uses wavfile.h and wavfile.cpp functions
	FILE * fp = wavfile_open("sound_new_wb.wav");
	wavfile_write(fp, buf, num_items);
	wavfile_close(fp);
	*/
	//computing FFT for 128 samples
	float mag[16384] = {0};

	for (i = 0; i < N; i++)
	{
		in[i] = buf[i];
	}
	if ((cfg = kiss_fftr_alloc(N, 0/*is_inverse_fft*/, NULL, NULL)) != NULL)
	{
		ptimer1 = PAPI_get_real_usec();
		kiss_fftr(cfg, in, out);
		ptimer2 = PAPI_get_real_usec();
		free(cfg);

		/*for(k=0;k<=N/2;k++)
		{
			printf("real[%d] = %f, imag[%d] = %f\n",k,out[k].r,k,out[k].i);
		}*/
		printf("FFT output \n"); 
		for (k = 0; k < N; k++)
		{
			mag[k] = sqrt((out[k].r)*(out[k].r) + (out[k].i)*(out[k].i));
		}
		for (k = 0; k < N; k++)
		{
			printf("%f\n",mag[k]);
		}
		printf("Time elapsed is (PAPI)%llu\n",(ptimer2-ptimer1));

	}
	else
	{
		printf("err\n");
	}

	
	return 0;
}


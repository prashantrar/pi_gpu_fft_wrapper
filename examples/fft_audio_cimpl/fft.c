
#include "wavfile.h"
#include <sndfile.h>
#include <malloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<fftw3.h>
#include "papi.h"


// myfft.cpp : Defines the entry point for the console application.
//

#include <math.h>

#define SIZE 128
#define N 16384

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
	int i, j;
	FILE *fileout;
	FILE *out_file;

	fftw_complex *in, *out;
	fftw_plan p;
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
	p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

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
	short int dir = 1;
	long m = 14;
	for (i = 0; i < N; i++)
	{
		in[i][0] = buf[i];
		in[i][1] = 0;
	}
	ptimer1 = PAPI_get_real_usec();
	fftw_execute(p);
	ptimer2 = PAPI_get_real_usec();
	printf("FFT output \n");
	for (i = 0; i < N; i++)
	{
		mag[i] = sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]);
	}
	for (i = 0; i < N; i++)
	{
		printf("%f\n",mag[i]);
	}
	printf("Time elapsed is (PAPI)%llu\n",(ptimer2-ptimer1));

	return 0;
}


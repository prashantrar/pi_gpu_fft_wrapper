
#include "wavfile.h"
#include <sndfile.h>
#include <malloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include "mailbox.h"
#include "gpu_fft.h"
#include "papi.h"

#define SIZE 128

int main()
{
	SNDFILE *sf;
	SF_INFO info;
	int num_channels;
	int num, num_items;
	//short *buf;
	float *buf;
	int f, sr, c;
	int i, j, k, ret, jobs, N, mb = mbox_open();
	long long ptimer1 = 0;
	long long ptimer2 = 0;
	struct GPU_FFT_COMPLEX *base;
    	struct GPU_FFT *fft;
	FILE *out;
	FILE *out_file;
	jobs = 1;


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
	  fwrite(buf, sizeof(short), num_items, out);
	  fclose(out);

	 *Creating new .wav file --- uses wavfile.h and wavfile.cpp functions
	 FILE * fp = wavfile_open("sound_new_wb.wav");
	 wavfile_write(fp, buf, num_items);
	 wavfile_close(fp);
	 */
	//computing FFT for 128 samples
	float mag[16384] = {0};
	int log2_N = 14;
	N = 1<<log2_N; // FFT length
    	ret = gpu_fft_prepare(mb, log2_N, GPU_FFT_FWD, jobs, &fft); // call once
	switch(ret) {
		case -1: printf("Unable to enable V3D. Please check your firmware is up to date.\n"); return -1;
		case -2: printf("log2_N=%d not supported.  Try between 8 and 22.\n", log2_N);         return -1;
		case -3: printf("Out of memory.  Try a smaller batch or increase GPU memory.\n");     return -1;
		case -4: printf("Unable to map Videocore peripherals into ARM memory space.\n");      return -1;
		case -5: printf("Can't open libbcm_host.\n");                                         return -1;
	}
	for (j=0; j<jobs; j++) {
		base = fft->in + j*fft->step; // input buffer
		for (i=0; i<N; i++){ base[i].re = buf[i]; base[i].im = 0;}//giving input to fft
	}

	usleep(1); // Yield to OS
	ptimer1 = PAPI_get_real_usec();
	gpu_fft_execute(fft); // call one or many times
	ptimer2 = PAPI_get_real_usec();


	printf("FFT output \n");
	for (j=0; j<jobs; j++) {
		base = fft->out + j*fft->step; // output buffer
		for (i=0; i<N; i++) {
			mag[i] = sqrt((base[i].re)*(base[i].re) + (base[i].im)*(base[i].im));		
		}
	}
	for (i = 0; i < N; i++)
	{
		printf("%f\n",mag[i]);
	}
	gpu_fft_release(fft); // Videocore memory lost if not freed !
	printf("Time elapsed is (PAPI)%llu\n",(ptimer2-ptimer1));
	return 0;
}


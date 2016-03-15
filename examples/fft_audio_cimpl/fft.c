
#include "wavfile.h"
#include <sndfile.h>
#include <malloc.h>

#include <stdio.h>
#include <stdlib.h>
//#include <process.h>
#include <unistd.h>


// myfft.cpp : Defines the entry point for the console application.
//

#include <math.h>

#define SIZE 128

/*
This computes an in-place complex-to-complex FFT
x and y are the real and imaginary arrays of 2^m points.
dir =  1 gives forward transform
dir = -1 gives reverse transform
*/
int FFT(short int dir, long m, float *x, float *y)
{
	long n, i, i1, j, k, i2, l, l1, l2;
	double c1, c2, tx, ty, t1, t2, u1, u2, z;

	/* Calculate the number of points */
	n = 1;
	for (i = 0; i<m; i++)
		n *= 2;

	/* Do the bit reversal */
	i2 = n >> 1;
	j = 0;
	for (i = 0; i<n - 1; i++) {
		if (i < j) {
			tx = x[i];
			ty = y[i];
			x[i] = x[j];
			y[i] = y[j];
			x[j] = tx;
			y[j] = ty;
		}
		k = i2;
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	/* Compute the FFT */
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for (l = 0; l<m; l++) {
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j = 0; j<l1; j++) {
			for (i = j; i<n; i += l2) {
				i1 = i + l1;
				t1 = u1 * x[i1] - u2 * y[i1];
				t2 = u1 * y[i1] + u2 * x[i1];
				x[i1] = x[i] - t1;
				y[i1] = y[i] - t2;
				x[i] += t1;
				y[i] += t2;
			}
			z = u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		if (dir == 1)
			c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}

	/* Scaling for forward transform */
	/*
	if (dir == 1) {
	for (i = 0; i<n; i++) {
	x[i] /= n;
	y[i] /= n;
	}
	}
	*/
	for (i = 0; i < SIZE; i++)
	{
		printf("x[%d] = %d y[%d] = %d \n", i, x[i], i, y[i]);
	}


	return(1);
}

int main()
{
	SNDFILE *sf;
	SF_INFO info;
	int num_channels;
	int num, num_items;
	//short *buf;
	float *buf;
	int f, sr, c;
	int i, j;
	FILE *out;
	FILE *out_file;

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
	float x[16384] = {0};
	float y[16384] = {0};
	float mag[16384] = {0};
	short int dir = 1;
	long m = 14;
	for (i = 0; i < num_items; i++)
	{
		x[i] = buf[i];
		//y[i] = 0;
	}
	FFT(dir, m, x, y);
	printf("FFT output \n");
	for (i = 0; i < num_items; i++)
	{
		mag[i] = sqrt(x[i]*x[i] + y[i]*y[i]);
		//y[i] = 0;
	}
	for (i = 0; i < num_items; i++)
	{
		printf("%f\n",mag[i]);
	}

//	getchar();
	return 0;
}


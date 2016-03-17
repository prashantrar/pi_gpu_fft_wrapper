

fft_test_direct:fft_test_direct.c
	gcc -I../ -L../ -Wl,-rpath=../ -o fft_test_direct.out fft_test_direct.c -lgpufft -lm -ldl -lrt -Ofast /usr/local/lib/libpapi.a


clean:
	rm -rf *.out *.txt

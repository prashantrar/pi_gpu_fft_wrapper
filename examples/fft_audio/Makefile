

fft_audio:fft_audio.c
	gcc -I../../ -L../../ -Wl,-rpath=../../ -o fft_audio.out fft_audio.c -lgpufft -lm -ldl -lrt -lsndfile -Ofast -ffast-math -fomit-frame-pointer /usr/local/lib/libpapi.a


clean:
	rm -rf *.out *.txt

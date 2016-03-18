

fft_audio:fft_audio_kiss.c
	gcc -I./Kiss_fft -o fft_audio_kiss.out fft_audio_kiss.c ./Kiss_fft/kiss_fft.c ./Kiss_fft/kiss_fftr.c -lm -lsndfile -Ofast -ffast-math -fomit-frame-pointer /usr/local/lib/libpapi.a


clean:
	rm -rf *.out *.txt

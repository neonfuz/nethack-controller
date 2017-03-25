
main: src/main.c
	$(CC) $(CFLAGS) -o main src/main.c `sdl2-config --cflags --libs`

clean:
	rm main

.PHONY: clean

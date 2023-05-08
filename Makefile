default: main.c
	clang main.c -O3 -o snake `sdl2-config --cflags` `sdl2-config --libs` -lSDL2_ttf

run: snake
	./snake

clean:
	rm -f snake

default: main.c
	gcc main.c -o snake_game `sdl2-config --cflags` `sdl2-config --libs` -lSDL2_ttf

run: snake_game
	./snake_game

clean:
	rm -f snake_game

CC=clang
CFLAGS=-O3 -Wall $(shell pkg-config --cflags --libs SDL2 SDL2_ttf)

default: src/main.c clean
	mkdir -p dist/
	$(CC) $(CFLAGS) -o dist/snake src/main.c

run: dist/snake
	./dist/snake

clean:
	rm -rf dist/

.PHONY: clean

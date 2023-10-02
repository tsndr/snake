CC=clang
CFLAGS=-O3 -Wall -Wextra -ggdb $(shell pkg-config --cflags --libs SDL2 SDL2_ttf)
SRC=src/main.c
DIST=dist/snake

default: $(SRC) clean
	mkdir -p dist/
	$(CC) $(CFLAGS) -o $(DIST) $(SRC)

run: $(DIST)
	./$(DIST)

clean:
	rm -rf dist/

.PHONY: clean

all: build run
build: jogo.c
	gcc jogo.c -o jogo -lSDL2 -lSDL2_image -lSDL2_ttf
run: jogo
	./jogo
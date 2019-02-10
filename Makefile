all: RPGInC

RPGInC:
	gcc -g -o RPGInC RPGInC.c RPGInC.h `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf

run:
	./RPGInC

map:
	gcc -g -o MAPCreater MAPCreater.c `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf

runmap:
	./MAPCreater

clean:
	rm -f RPGInC

all: RPGInC

RPGInC: effect.o sounds.o load.o
	gcc -g -o RPGInC RPGInC.c RPGInC.h effect.o sounds.o load.o `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

effect.o:
	gcc -g -c ./effect/effect.c ./effect/effect.h

sounds.o:
	gcc -g -c ./sounds/sounds.c ./sounds/sounds.h

load.o:
	gcc -g -c ./load/load.c ./load/load.h

run:
	./RPGInC

map:
	gcc -g -o MAPCreater MAPCreater.c `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf

runmap:
	./MAPCreater

clean:
	rm -f RPGInC
	rm -f effect.o
	rm -f sounds.o
	rm -f load.o

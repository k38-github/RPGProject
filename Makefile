all: RPGInC

RPGInC: effect.o
	gcc -g -o RPGInC RPGInC.c RPGInC.h effect.o `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

effect.o:
	gcc -g -c ./effect/effect.c ./effect/effect.h

run:
	./RPGInC

map:
	gcc -g -o MAPCreater MAPCreater.c `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf

runmap:
	./MAPCreater

clean:
	rm -f RPGInC
	rm -f effect.o

all: RPGInC

RPGInC:
	gcc -g -o RPGInC RPGInC.c RPGInC.h `sdl2-config --cflags --libs` -lSDL2_image

run:
	./RPGInC

map:
	gcc -g -o MAPCreater MAPCreater.c `sdl2-config --cflags --libs` -lSDL2_image

runmap:
	./MAPCreater

clean:
	rm -f RPGInC

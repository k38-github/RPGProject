all: RPGInC

RPGInC:
	gcc -g -o RPGInC RPGInC.c RPGInC.h `sdl2-config --cflags --libs` -lSDL2_image


run:
	./RPGInC


clean:
	rm -f RPGInC

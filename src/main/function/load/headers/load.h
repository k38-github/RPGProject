#ifndef LOAD_H
#define LOAD_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct {
    int mapchip_id;
    char mapchip_name[256];
    int movable;
    int change_locate;
    SDL_Texture *map_image;
} MAPCHIP;

extern MAPCHIP mapchip[256];

int load_image(SDL_Renderer *, SDL_Texture **, char *);
int load_mapchip(SDL_Renderer *, MAPCHIP *, int);
int load_map(char *, int *, int *, int *, int *);

#endif

#ifdef _RPGINC
#else

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef enum {DOWN, LEFT, RIGHT, UP} DIRECTION;
typedef enum {FALSE, TRUE} MOVING;

typedef struct {
    int map_x;
    int map_y;
    int pixel_x;
    int pixel_y;
    int offset_x;
    int offset_y;
    int velocity_x;
    int velocity_y;
    DIRECTION direction;
    MOVING moving;
} CARACTER;

typedef struct {
    int mapchip_id;
    char mapchip_name[256];
    int movable;
    int change_locate;
    SDL_Texture *map_image;
} MAPCHIP;


int load_image(SDL_Renderer *, SDL_Texture **, char *);
int character_animation(SDL_Renderer *);
int character_update(SDL_Renderer *, SDL_Event);
int character_move(SDL_Event);

int load_map_image(SDL_Renderer *, SDL_Texture **);
int mapchip_load(SDL_Renderer *);
int load_event(SDL_Renderer *);
int load_map(char *);
int draw_map(SDL_Renderer *);
int is_movable(int, int);
int clac_offset(int, int, int *, int *);
int fade_out(SDL_Renderer *);

#endif

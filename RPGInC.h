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
    CARACTER npc;
    char message[1024];
    SDL_Texture *npc_image;
    MOVING npc_move;
} NPC;

typedef struct {
    int mapchip_id;
    char mapchip_name[256];
    int movable;
    int change_locate;
    SDL_Texture *map_image;
} MAPCHIP;


int clac_offset(int, int, int *, int *);
int load_image(SDL_Renderer *, SDL_Texture **, char *);
int player_animation(SDL_Renderer *);
int player_update(SDL_Renderer *, SDL_Event);
int player_move(SDL_Event);

int load_npc(SDL_Renderer *);
int npc_animation(SDL_Renderer *);
int npc_update(SDL_Renderer *renderer, int);
int npc_move(DIRECTION, int);

int load_map_image(SDL_Renderer *, SDL_Texture **);
int load_mapchip(SDL_Renderer *);
int load_move(SDL_Renderer *);
int load_map(char *);
int draw_map(SDL_Renderer *);
int is_movable(int, int);
int fade_out(SDL_Renderer *);

#endif

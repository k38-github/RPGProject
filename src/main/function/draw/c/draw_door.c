#include <stdio.h>
#include <SDL2/SDL.h>
#include "../headers/draw_door.h"
#include "../../load/headers/load.h"
#include "../../parts.h"

int draw_door(SDL_Renderer *renderer, SDL_Event e){

    int i;

    for(i = 0;i<sizeof(door)/sizeof(door[0]);i++){
        if (door[i].status != 9) {
            SDL_Rect imageRect=(SDL_Rect){0, 0, IMAGE_WIDTH, IMAGE_HEIGHT};
            SDL_Rect drawRect=(SDL_Rect){(door[i].map_x * GRID_SIZE) - player.offset_x,
                                         (door[i].map_y * GRID_SIZE) - player.offset_y,
                                         IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

            if (door[i].status == 0){
                SDL_RenderCopy(renderer, mapchip[17].map_image, &imageRect, &drawRect);
            }
        }
    }

    return 0;
}

#include <stdio.h>
#include <SDL2/SDL.h>
#include "../headers/draw_treasure.h"
#include "../../load/headers/load.h"
#include "../../utils/headers/is_movable.h"
#include "../../parts.h"

int draw_treasure(SDL_Renderer *renderer){

    int i, j;

    for(i = 0;i<sizeof(treasure)/sizeof(treasure[0]);i++){
        if (strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {
            for(j = 0;j<sizeof(treasure->treasure)/sizeof(treasure->treasure[0]);j++){
                if (strcmp(treasure[i].treasure[j].item, "empty") != 0) {
                    SDL_Rect imageRect=(SDL_Rect){0, 0, IMAGE_WIDTH, IMAGE_HEIGHT};
                    SDL_Rect drawRect=(SDL_Rect){(treasure[i].treasure[j].map_x * GRID_SIZE) - player.offset_x,
                                                 (treasure[i].treasure[j].map_y * GRID_SIZE) - player.offset_y,
                                                 IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

                    if (treasure[i].treasure[j].status == 0){
                        SDL_RenderCopy(renderer, mapchip[15].map_image, &imageRect, &drawRect);
                    } else {
                        SDL_RenderCopy(renderer, mapchip[16].map_image, &imageRect, &drawRect);
                    }
                }
            }
        }
    }

    return 0;
}



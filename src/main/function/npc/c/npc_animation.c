#include <stdio.h>
#include <SDL2/SDL.h>
#include "../headers/npc_animation.h"
#include "../headers/npc_update.h"
#include "../../parts.h"

int npc_animation(SDL_Renderer *renderer) {

    int i;
    for(i = 0; i < number_of_npc_image;i++) {

        int x = ((frame / animecycle) % 4) * 16;
        int y = npc[i].npc.direction * IMAGE_HEIGHT;

        SDL_Rect imageRect=(SDL_Rect){x, y, IMAGE_WIDTH, IMAGE_HEIGHT};
        SDL_Rect drawRect=(SDL_Rect){npc[i].npc.pixel_x - player.offset_x,
                                     npc[i].npc.pixel_y - player.offset_y,
                                     IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

        SDL_RenderCopy(renderer, npc[i].npc_image, &imageRect, &drawRect);

        if (npc[i].npc_move == TRUE && number_of_npc_image != 0) {
            npc_update(renderer, i);
        }
    }

    return 0;
}

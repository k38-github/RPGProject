#include <stdio.h>
#include <SDL2/SDL.h>
#include "../headers/npc_move.h"
#include "../headers/npc_update.h"
#include "../../parts.h"

int npc_update(SDL_Renderer *renderer, int element) {

    srand((unsigned)time(NULL));
    int target = rand()%number_of_npc_image;
    int action = rand()%10;

    if (npc[element].npc_move == TRUE) {
        if (npc[element].npc.moving == TRUE) {
            npc[element].npc.pixel_x = npc[element].npc.pixel_x + npc[element].npc.velocity_x;
            npc[element].npc.pixel_y = npc[element].npc.pixel_y + npc[element].npc.velocity_y;

            if (npc[element].npc.pixel_x % GRID_SIZE == 0 && npc[element].npc.pixel_y % GRID_SIZE == 0) {
                npc[element].npc.moving = FALSE;
                npc[element].npc.map_x = npc[element].npc.pixel_x / GRID_SIZE;
                npc[element].npc.map_y = npc[element].npc.pixel_y / GRID_SIZE;
            }

        } else {
            if (target == element && action < 3) {
                npc_move(rand()%4, element);
            }
        }
    }

    return 0;
}

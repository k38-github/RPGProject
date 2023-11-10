#include <stdio.h>
#include "../headers/player_animation.h"
#include "../headers/player_move.h"
#include "../../battle/headers/battle_encount.h"
#include "../../load/headers/load_move.h"
#include "../../parts.h"

int player_update(SDL_Renderer *renderer, SDL_Event e, SDL_Texture *player_image) {

    if (player.moving == TRUE) {
        player.pixel_x = player.pixel_x + player.velocity_x;
        player.pixel_y = player.pixel_y + player.velocity_y;

        if (player.pixel_x % GRID_SIZE == 0 && player.pixel_y % GRID_SIZE == 0){
            player.moving = FALSE;
            player.map_x = player.pixel_x / GRID_SIZE;
            player.map_y = player.pixel_y / GRID_SIZE;


            load_move(renderer, player_image);

            if (number_of_monster != 0) {
                battle_encount(renderer, e);
            }

            player_move(e);

        }

    } else {
        player_move(e);
    }

    return 0;
}

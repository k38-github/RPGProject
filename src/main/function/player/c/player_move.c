#include <stdio.h>
#include "../headers/player_move.h"
#include "../../utils/headers/is_movable.h"
#include "../../parts.h"

int player_move(SDL_Event e) {

    if (message_window.visible == OUT_VISIBLE) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP){
            player.direction = UP;
            if (is_movable(player.map_x, player.map_y - 1) == TRUE) {
                player.velocity_x = 0;
                player.velocity_y = -speed;
                player.moving = TRUE;
            }
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN){
            player.direction = DOWN;
            if (is_movable(player.map_x, player.map_y + 1) == TRUE) {
                player.velocity_x = 0;
                player.velocity_y = speed;
                player.moving = TRUE;
            }
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT){
            player.direction = RIGHT;
            if (is_movable(player.map_x + 1, player.map_y) == TRUE) {
                player.velocity_x = speed;
                player.velocity_y = 0;
                player.moving = TRUE;
            }
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT){
            player.direction = LEFT;
            if (is_movable(player.map_x - 1, player.map_y) == TRUE) {
                player.velocity_x = -speed;
                player.velocity_y = 0;
                player.moving = TRUE;
            }
        }
    }

    return 0;

}

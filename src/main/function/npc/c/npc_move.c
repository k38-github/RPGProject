#include <stdio.h>
#include <SDL2/SDL.h>
#include "../headers/npc_move.h"
#include "../../utils/headers/is_movable.h"
#include "../../parts.h"

int npc_move(DIRECTION direction, int element) {

    if (message_window.visible == OUT_VISIBLE) {
        if (frame == 0) {
            if (direction == UP){
                npc[element].npc.direction = UP;
                if (is_movable(npc[element].npc.map_x, npc[element].npc.map_y - 1) == TRUE) {
                    npc[element].npc.velocity_x = 0;
                    npc[element].npc.velocity_y = -speed;
                    npc[element].npc.moving = TRUE;
                }
            } else if (direction == DOWN){
                npc[element].npc.direction = DOWN;
                if (is_movable(npc[element].npc.map_x, npc[element].npc.map_y + 1) == TRUE) {
                    npc[element].npc.velocity_x = 0;
                    npc[element].npc.velocity_y = speed;
                    npc[element].npc.moving = TRUE;
                }
            } else if (direction == RIGHT){
                npc[element].npc.direction = RIGHT;
                if (is_movable(npc[element].npc.map_x + 1, npc[element].npc.map_y) == TRUE) {
                    npc[element].npc.velocity_x = speed;
                    npc[element].npc.velocity_y = 0;
                    npc[element].npc.moving = TRUE;
                }
            } else if (direction == LEFT){
                npc[element].npc.direction = LEFT;
                if (is_movable(npc[element].npc.map_x - 1, npc[element].npc.map_y) == TRUE) {
                    npc[element].npc.velocity_x = -speed;
                    npc[element].npc.velocity_y = 0;
                    npc[element].npc.moving = TRUE;
                }
            }
        }
    }
    return 0;

}

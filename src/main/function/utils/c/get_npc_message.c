#include <stdio.h>
#include <SDL2/SDL.h>
#include "../headers/get_npc_message.h"
#include "../../parts.h"

int get_npc_message() {

    int i;

    for(i = 0;i < number_of_npc_image;i++) {
        if (player.direction == UP) {
            if (npc[i].npc.map_x == player.map_x && npc[i].npc.map_y == player.map_y - 1) {
                npc[i].npc.direction = DOWN;
                message = npc[i].message;
                return 0;
            }
        } else if (player.direction == DOWN) {
            if (npc[i].npc.map_x == player.map_x && npc[i].npc.map_y == player.map_y + 1) {
                npc[i].npc.direction = UP;
                message = npc[i].message;
                return 0;
            }
        } else if (player.direction == RIGHT) {
            if (npc[i].npc.map_x == player.map_x + 1 && npc[i].npc.map_y == player.map_y) {
                npc[i].npc.direction = LEFT;
                message = npc[i].message;
                return 0;
            }
        } else if (player.direction == LEFT) {
            if (npc[i].npc.map_x == player.map_x - 1 && npc[i].npc.map_y == player.map_y) {
                npc[i].npc.direction = RIGHT;
                message = npc[i].message;
                return 0;
            }
        }
    }

    message = TALK_MESSAGE;

    return 0;
}

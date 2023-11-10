#include <stdio.h>
#include "../headers/get_message.h"
#include "../headers/get_npc_message.h"
#include "../headers/get_treasure_message.h"
#include "../headers/is_movable.h"
#include "../headers/open_door.h"
#include "../headers/message_window_status.h"
#include "../../parts.h"

int get_message() {

    int i;

    message = SEARCH_MESSAGE;

    if (player.direction == UP) {
        if (is_movable(player.map_x, player.map_y - 1) == TALK) {
            get_npc_message();
        } else if (is_movable(player.map_x, player.map_y - 1) == SEARCH) {
            get_treasure_message();
        } else if (is_movable(player.map_x, player.map_y - 1) == OPEN) {
            open_door();
            message_window_status();
            state = OFF;
        }
    } else if (player.direction == DOWN) {
        if (is_movable(player.map_x, player.map_y + 1) == TALK) {
            get_npc_message();
        } else if (is_movable(player.map_x, player.map_y + 1) == SEARCH) {
            get_treasure_message();
        } else if (is_movable(player.map_x, player.map_y + 1) == OPEN) {
            open_door();
            message_window_status();
            state = OFF;
        }
    } else if (player.direction == RIGHT) {
        if (is_movable(player.map_x + 1, player.map_y) == TALK) {
            get_npc_message();
        } else if (is_movable(player.map_x + 1, player.map_y) == SEARCH) {
            get_treasure_message();
        } else if (is_movable(player.map_x + 1, player.map_y) == OPEN) {
            open_door();
            message_window_status();
            state = OFF;
        }
    } else if (player.direction == LEFT) {
        if (is_movable(player.map_x - 1, player.map_y) == TALK) {
            get_npc_message();
        } else if (is_movable(player.map_x - 1, player.map_y) == SEARCH) {
            get_treasure_message();
        } else if (is_movable(player.map_x - 1, player.map_y) == OPEN) {
            open_door();
            message_window_status();
            state = OFF;
        }
    }

    return 0;

}

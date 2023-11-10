#include <stdio.h>
#include "../headers/open_door.h"
#include "../headers/message_window_status.h"
#include "../../sounds/headers/sounds.h"
#include "../../parts.h"

int open_door() {
    int i;
    char *se_file = "door.ogg";

    for(i = 0;i<sizeof(door)/sizeof(door[0]);i++){
        if (player.direction == UP) {
            if (door[i].map_x == player.map_x && door[i].map_y == player.map_y - 1) {
                door[i].status = 1;
                sound_se(se_file);
                return 0;
            }
        } else if (player.direction == DOWN) {
            if (door[i].map_x == player.map_x && door[i].map_y == player.map_y + 1) {
                door[i].status = 1;
                sound_se(se_file);
                return 0;
            }
        } else if (player.direction == RIGHT) {
            if (door[i].map_x == player.map_x + 1 && door[i].map_y == player.map_y) {
                door[i].status = 1;
                sound_se(se_file);
                return 0;
            }
        } else if (player.direction == LEFT) {
            if (door[i].map_x == player.map_x - 1 && door[i].map_y == player.map_y) {
                door[i].status = 1;
                sound_se(se_file);
                return 0;
            }
        }
    }

    message_window_status();
    message = DOOR_MESSAGE;

    return 0;
}

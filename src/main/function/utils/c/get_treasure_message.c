#include <stdio.h>
#include "../headers/get_treasure_message.h"
#include "../../sounds/headers/sounds.h"
#include "../../parts.h"

int get_treasure_message() {

    int i, j;
    char *se_file = "treasure.ogg";

    for (i = 0;i < sizeof(treasure)/sizeof(treasure[0]);i++) {
        for (j = 0;j < sizeof(treasure->treasure)/sizeof(treasure->treasure[0]);j++) {
            if (player.direction == UP) {
                if (treasure[i].treasure[j].map_x == player.map_x &&
                    treasure[i].treasure[j].map_y == player.map_y - 1 &&
                    strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {

                    sound_se(se_file);

                    if (treasure[i].treasure[j].status == 0) {
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 24);
                        treasure[i].treasure[j].status = 1;
                    } else {
                        message = TRESUREBOX_EMPTY_MESSAGE;
                    }

                    return 0;
                }
            } else if (player.direction == DOWN) {
                if (treasure[i].treasure[j].map_x == player.map_x &&
                    treasure[i].treasure[j].map_y == player.map_y + 1 &&
                    strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {

                    sound_se(se_file);

                    if (treasure[i].treasure[j].status == 0) {
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 24);
                        treasure[i].treasure[j].status = 1;
                    } else {
                        message = TRESUREBOX_EMPTY_MESSAGE;
                    }

                    return 0;

                }
           } else if (player.direction == RIGHT) {
                if (treasure[i].treasure[j].map_x == player.map_x + 1 &&
                    treasure[i].treasure[j].map_y == player.map_y &&
                    strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {

                    sound_se(se_file);

                    if (treasure[i].treasure[j].status == 0) {
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 24);
                        treasure[i].treasure[j].status = 1;
                    } else {
                        message = TRESUREBOX_EMPTY_MESSAGE;
                    }

                    return 0;

                }
           } else if (player.direction == LEFT) {
                if (treasure[i].treasure[j].map_x == player.map_x - 1 &&
                    treasure[i].treasure[j].map_y == player.map_y &&
                    strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {

                    sound_se(se_file);

                    if (treasure[i].treasure[j].status == 0) {
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 24);
                        treasure[i].treasure[j].status = 1;
                    } else {
                        message = TRESUREBOX_EMPTY_MESSAGE;
                    }

                    return 0;

                }
            }
        }
    }

    message = SEARCH_MESSAGE;

    return 0;
}

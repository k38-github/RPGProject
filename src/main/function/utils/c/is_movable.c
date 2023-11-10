#include <stdio.h>
#include "../headers/is_movable.h"
#include "../../sounds/headers/sounds.h"
#include "../../load/headers/load.h"
#include "../../parts.h"

int is_movable(int x, int y) {

    int i, j;
    for (i = 0;i < number_of_npc_image;i++) {
        if (npc[i].npc.map_x == x && npc[i].npc.map_y == y) {
            return TALK;
        }
    }

    for (i = 0;i < sizeof(treasure)/sizeof(treasure[0]);i++) {
        for (j = 0;j < sizeof(treasure->treasure)/sizeof(treasure->treasure[0]);j++) {
            if (treasure[i].treasure[j].map_x == x && treasure[i].treasure[j].map_y == y &&
                strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {
                return SEARCH;
            }
        }
    }

    for (i = 0;i < sizeof(door)/sizeof(door[0]);i++) {
        if (door[i].map_x == x && door[i].map_y == y && door[i].status == 0) {
            return OPEN;
        }
    }

    if ( x < 0 || x > COL - 1 || y  < 0 || y > ROW - 1) {
        sound_se("collisionOfWall.ogg");
        return 2;
    }

    if (mapchip[map_array[y*COL+x]].movable == 1) {
        sound_se("collisionOfWall.ogg");
        return 2;
    }

    if (player.map_x == x && player.map_y == y) {
        sound_se("collisionOfWall.ogg");
        return 1;
    }

    return TRUE;
}

#include <stdio.h>
#include "../headers/load.h"
#include "../headers/load_monster.h"
#include "../headers/load_move.h"
#include "../headers/load_treasure.h"
#include "../headers/load_door.h"
#include "../headers/load_npc.h"
#include "../../utils/headers/fade_out.h"
#include "../../sounds/headers/sounds.h"
#include "../../parts.h"

int load_move(SDL_Renderer *renderer, SDL_Texture *player_image) {
    char event_path[256] = {0};

    sprintf(event_path, "src/resources/evt/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256] = {0};
    int event_point_x;
    int event_point_y;
    DIRECTION direction_of_penetration;
    char buf[256] = {0};
    char new_map_name[256] = {0};
    char map_path[256] = {0};
    int new_x;
    int new_y;
    int i = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "MOVE", 4) == 0) {
                sscanf(buf,
                   "%[^,],%d,%d,%u,%[^,],%d,%d",
                       event, &event_point_x, &event_point_y, &direction_of_penetration, new_map_name, &new_x, &new_y);

                if (player.map_x == event_point_x && player.map_y == event_point_y) {
                    if (player.direction == direction_of_penetration) {

                        load_se(player.map_x, player.map_y, MAP_EVENT_NAME);

                        sprintf(MAP_EVENT_NAME, "%s", new_map_name);

                        sprintf(map_path, "src/resources/map/%s.map", new_map_name);
                        load_map(map_path, map_array, &COL, &ROW, &OUT_OF_MAP);
                        load_treasure(renderer);
                        load_door(renderer);

                        load_bgm(MAP_EVENT_NAME);

                        player.map_x = new_x;
                        player.map_y = new_y;
                        player.pixel_x = player.map_x * GRID_SIZE;
                        player.pixel_y = player.map_y * GRID_SIZE;

                        load_npc(renderer);
                        load_monster(renderer);

                        fade_out(renderer, player_image);

                        break;
                    }
                }
            }
        }
    }

    fclose(fp);

    return 0;
}



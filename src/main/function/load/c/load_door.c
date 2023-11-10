#include <stdio.h>
#include "../headers/load_treasure.h"
#include "../../parts.h"

int load_door(SDL_Renderer *renderer) {
    char event_path[256] = {0};

    sprintf(event_path, "src/resources/evt/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256] = {0};
    int map_x;
    int map_y;

    char buf[256] = {0};
    int i;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for (i = 0;i < sizeof(door)/sizeof(door[0]);i++) {
        door[i].status = 9;
    }

    for (i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "DOOR", 4) == 0) {
                sscanf(buf,
                   "%[^,],%d,%d",
                       event, &map_x, &map_y);

                 door[i].map_x = map_x;
                 door[i].map_y = map_y;
                 door[i].status = 0;

            }
        }
    }


    fclose(fp);

    return 0;
}

#include <stdio.h>
#include "../headers/load_treasure.h"
#include "../../parts.h"

int load_treasure(SDL_Renderer *renderer) {
    char event_path[256] = {0};

    sprintf(event_path, "src/resources/evt/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256] = {0};
    int item_id;
    int map_x;
    int map_y;
    char item[128] = {0};
    int item_length;

    char buf[256] = {0};
    int i, j;
    int new_flg = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for (i = 0;i < sizeof(treasure)/sizeof(treasure[0]);i++) {
        if (strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {
            new_flg = 1;
        }
    }

    if (new_flg == 0) {
        i = 0;
        while (1) {
            if (strcmp(treasure[i].map, "empty") == 0) {
                sprintf(treasure[i].map, "%s", MAP_EVENT_NAME);
                break;
            }
            i++;
        }
    }

    for (i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "TREASURE", 8) == 0) {
                sscanf(buf,
                   "%[^,],%d,%d,%d,%[^,]",
                       event, &item_id, &map_x, &map_y, item);

                for (j = 0;j < sizeof(treasure)/sizeof(treasure[0]);j++) {
                    if (strcmp(treasure[j].map, MAP_EVENT_NAME) == 0) {
                        if (treasure[j].treasure[item_id].status != 1) {
                            treasure[j].treasure[item_id].map_x = map_x;
                            treasure[j].treasure[item_id].map_y = map_y;
                            treasure[j].treasure[item_id].status = 0;

                            item_length = strlen(item);
                            item[item_length - 1] = '\0';
                            sprintf(treasure[j].treasure[item_id].item, "%s", item);
                        }
                    }
                }

            }
        }
    }


    fclose(fp);

    return 0;
}



#include <stdio.h>
#include "../headers/load.h"
#include "../headers/load_npc.h"
#include "../../parts.h"

int load_npc(SDL_Renderer *renderer) {
    char event_path[256] = {0};

    sprintf(event_path, "src/resources/evt/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256] = {0};
    char npc_name[256] = {0};
    int map_x;
    int map_y;
    DIRECTION direction;
    MOVING moving;
    int max_step;
    int message_length;
    char message[1024] = {0};

    char buf[1024] = {0};
    char npc_path[256] = {0};
    int i = 0;
    int element_number = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for (i = 0;i < number_of_npc_image;i++) {
        npc[i].npc.map_x = 0;
        npc[i].npc.map_y = 0;
        npc[i].npc.pixel_x = 0;
        npc[i].npc.pixel_y = 0;
        npc[i].npc.direction = 0;
        npc[i].npc.moving = 0;
        SDL_DestroyTexture(npc[i].npc_image);
    }

    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "CHARA", 5) == 0) {
                sscanf(buf,
                   "%[^,],%[^,],%d,%d,%u,%u,%[^,]",
                       event, npc_name, &map_x, &map_y, &direction, &moving, message);

                sprintf(npc_path, "src/resources/image/charachip/%s.bmp", npc_name);
                load_image(renderer, &npc[element_number].npc_image, npc_path);

                npc[element_number].npc.map_x = map_x;
                npc[element_number].npc.map_y = map_y;
                npc[element_number].npc.pixel_x = map_x * GRID_SIZE;
                npc[element_number].npc.pixel_y = map_y * GRID_SIZE;
                npc[element_number].npc.direction = direction;
                npc[element_number].npc_move = moving;

                message_length = strlen(message);
                message[message_length - 1] = '\0';
                sprintf(npc[element_number].message, "%s", message);

                element_number += 1;
            }
        }
    }

    number_of_npc_image = element_number;

    fclose(fp);

    return 0;
}

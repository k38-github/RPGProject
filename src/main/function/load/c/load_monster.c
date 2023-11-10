#include <stdio.h>
#include "../headers/load_monster.h"
#include "../headers/load_monster_status.h"
#include "../../parts.h"

int load_monster(SDL_Renderer *renderer) {
    char event_path[256] = {0};

    sprintf(event_path, "src/resources/evt/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256] = {0};
    char monster_name[256] = {0};

    char buf[1024] = {0};
    char npc_path[256] = {0};
    int monster_id = 0;
    int i = 0;
    int element_number = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for (i = 0;i < 256;i++) {
        monster[i].monster_id = 0;
        strcpy(monster[i].status.name, "\0");
        monster[i].status.sex = 0;
        monster[i].status.level = 0;
        monster[i].status.hp = 0;
        monster[i].status.mp = 0;
        monster[i].status.strength = 0;
        monster[i].status.agility = 0;
        monster[i].status.protection = 0;
        monster[i].status.wisdom = 0;
        monster[i].status.luck = 0;
        strcpy(monster[i].skill1, "\0");
        strcpy(monster[i].skill2, "\0");
        strcpy(monster[i].skill3, "\0");
        strcpy(monster[i].skill4, "\0");
        strcpy(monster[i].skill5, "\0");
        strcpy(monster[i].item1, "\0");
        monster[i].item1_probability = 0;
        strcpy(monster[i].item2, "\0");
        monster[i].item2_probability = 0;
        strcpy(monster[i].item3, "\0");
        monster[i].item3_probability = 0;
        monster[i].gold = 0;
        monster[i].status.experience_point = 0;
    }

    for (i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "MONSTER", 7) == 0) {
                sscanf(buf, "%[^,],%[^,],%d", event, monster_name, &monster_id);

                load_monster_status(monster_name, monster_id, element_number);
                element_number++;
            }
        }

    }

    number_of_monster = element_number;

    fclose(fp);

    return 0;
}

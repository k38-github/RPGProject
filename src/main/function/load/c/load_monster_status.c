#include <stdio.h>
#include "../headers/load_monster_status.h"
#include "../../parts.h"

int load_monster_status(char *in_monster_name, int in_monster_id, int element_number) {

    char *event_path =  "src/resources/dat/monster.dat";

    FILE *fp;
    char buf[1024] = {0};
    char event[256] = {0};

    int monster_id = 0;
    char monster_name[256] = {0};
    int sex = 0;
    int level = 0;
    int hp = 0;
    int mp = 0;
    int strength = 0;
    int agility = 0;
    int protection = 0;
    int wisdom = 0;
    int luck = 0;
    char skill1[256] = {0};
    char skill2[256] = {0};
    char skill3[256] = {0};
    char skill4[256] = {0};
    char skill5[256] = {0};
    char item1[256] = {0};
    double item1_probability = 0.0;
    char item2[256] = {0};
    double item2_probability = 0.0;
    char item3[256] = {0};
    double item3_probability = 0.0;
    int gold = 0;
    int experience_point = 0;
    double encounter_probability = 0.0;

    int i = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {
        if (strncmp(buf, "#", 1) != 0){
           sscanf(buf,
           "%d,%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%lf,%[^,],%lf,%[^,],%lf,%d,%d, %lf",
           &monster_id, monster_name,
           &sex, &level, &hp, &mp, &strength, &agility, &protection, &wisdom, &luck,
           skill1, skill2, skill3, skill4, skill5,
           item1, &item1_probability, item2, &item2_probability, item3, &item3_probability,
           &gold, &experience_point, &encounter_probability);

            if (strcmp(in_monster_name, monster_name) == 0 && in_monster_id ==  monster_id) {
                monster[element_number].monster_id = monster_id;
                strcpy(monster[element_number].status.name, monster_name);
                monster[element_number].status.sex = sex;
                monster[element_number].status.level = level;
                monster[element_number].status.hp = hp;
                monster[element_number].status.mp = mp;
                monster[element_number].status.strength = strength;
                monster[element_number].status.agility = agility;
                monster[element_number].status.protection = protection;
                monster[element_number].status.wisdom = wisdom;
                monster[element_number].status.luck = luck;
                strcpy(monster[element_number].skill1, skill1);
                strcpy(monster[element_number].skill2, skill2);
                strcpy(monster[element_number].skill3, skill3);
                strcpy(monster[element_number].skill4, skill4);
                strcpy(monster[element_number].skill5, skill5);
                strcpy(monster[element_number].item1, item1);
                monster[element_number].item1_probability = item1_probability;
                strcpy(monster[element_number].item2, item2);
                monster[element_number].item2_probability = item2_probability;
                strcpy(monster[element_number].item3, item3);
                monster[element_number].item3_probability = item3_probability;
                monster[element_number].gold = gold;
                monster[element_number].status.experience_point = experience_point;
                monster[element_number].encounter_probability = encounter_probability;
            }
        }
    }

    // printf("name:%s\n", monster[0].status.name);
    // printf("name:%s\n", monster[1].status.name);
    // printf("name:%s\n", monster[2].status.name);
    // printf("ddd:%d %s %d %d %d %d %d %d %d %d %d %s %s %s %s %s %s %lf %s %lf %s %lf %d %d %lf\n",
    // monster[element_number].monster_id, monster[element_number].status.name,
    // monster[element_number].status.sex, monster[element_number].status.level,
    // monster[element_number].status.hp, monster[element_number].status.mp,
    // monster[element_number].status.strength,
    // monster[element_number].status.agility,
    // monster[element_number].status.protection,
    // monster[element_number].status.wisdom,
    // monster[element_number].status.luck,
    // monster[element_number].skill1,
    // monster[element_number].skill2,
    // monster[element_number].skill3,
    // monster[element_number].skill4,
    // monster[element_number].skill5,
    // monster[element_number].item1,
    // monster[element_number].item1_probability,
    // monster[element_number].item2,
    // monster[element_number].item2_probability,
    // monster[element_number].item3,
    // monster[element_number].item3_probability,
    // monster[element_number].gold, monster[element_number].status.experience_point,
    // monster[element_number].encounter_probability
    // );

    return 0;
}

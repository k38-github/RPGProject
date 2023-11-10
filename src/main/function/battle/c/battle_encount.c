#include <stdio.h>
#include "../headers/battle_encount.h"
#include "../headers/battle_window.h"
#include "../../parts.h"

int battle_encount(SDL_Renderer *renderer, SDL_Event e) {

    int target = rand()%number_of_monster;
    srand((unsigned)time(NULL));
    double encount = (double)rand()/RAND_MAX;

    // printf("encount:%lf\n", encount);
    // printf("monster:%lf\n", monster[target].encounter_probability);
    if (encount < monster[target].encounter_probability) {
        battle_window(renderer, e, monster[target]);
    } else {
        // printf("not encount\n");
    }

    return 0;
}

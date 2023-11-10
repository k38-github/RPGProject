#include <stdio.h>
#include "../headers/compare_agility.h"
#include "../../parts.h"

int compare_agility(const void * n1, const void * n2) {
    if (((MONSTER *)n1)->status.agility > ((MONSTER *)n2)->status.agility) {
        return -1;
    } else if (((MONSTER *)n1)->status.agility < ((MONSTER *)n2)->status.agility) {
        return 1;
    } else {
        return 0;
    }
}

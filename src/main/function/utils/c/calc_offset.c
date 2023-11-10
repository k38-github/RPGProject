#include <stdio.h>
#include "../headers/calc_offset.h"
#include "../../parts.h"

int calc_offset(int x, int y, int *offset_x, int *offset_y) {
    *offset_x = x - (SCREEN_WIDTH / 2);
    *offset_y = y - (SCREEN_HEIGHT / 2);

    return 0;
}

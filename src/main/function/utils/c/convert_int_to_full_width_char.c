#include <stdio.h>
#include <SDL2/SDL.h>
#include "../headers/convert_int_to_full_width_char.h"
#include "../../parts.h"

int convert_int_to_full_width_char(int num, char *str_to_return) {

    int i;
    int number;
    int digit = 0;
    int reverse = 0;
    int calc_digit;
    char tmp[256] = {};

    if (num == 0) {
        sprintf(str_to_return, "%s", "０");
        return 0;
    }

    num = num * 10 + 9;

    while (num > 0) {
        reverse = reverse * 10 + num % 10;
        num = num / 10;
    }

    calc_digit = reverse;

    while(calc_digit != 0) {
        calc_digit = calc_digit / 10;
        ++digit;
    } 

    for (i = 0;i < digit - 1;i++) {
        number = reverse % 10;

        if (number == 9) {
            sprintf(tmp, "%s%s", tmp , "９");
        } else if (number == 8) {
            sprintf(tmp, "%s%s", tmp , "８");
        } else if (number == 7) {
            sprintf(tmp, "%s%s", tmp , "７");
        } else if (number == 6) {
            sprintf(tmp, "%s%s", tmp , "６");
        } else if (number == 5) {
            sprintf(tmp, "%s%s", tmp , "５");
        } else if (number == 4) {
            sprintf(tmp, "%s%s", tmp , "４");
        } else if (number == 3) {
            sprintf(tmp, "%s%s", tmp , "３");
        } else if (number == 2) {
            sprintf(tmp, "%s%s", tmp , "２");
        } else if (number == 1) {
            sprintf(tmp, "%s%s", tmp , "１");
        } else if (number == 0) {
            sprintf(tmp, "%s%s", tmp , "０");
        }

        reverse = reverse / 10;

    }

    sprintf(str_to_return, "%s", tmp);

    return 0;
}



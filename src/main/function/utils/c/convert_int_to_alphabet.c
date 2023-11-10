#include <stdio.h>
#include "../headers/convert_int_to_alphabet.h"

int convert_int_to_alphabet(int number, char *alphabet_to_return) {

    int i;
    char tmp[10] = {0};

    if (number == 0) {
        sprintf(tmp, "%s", "Ａ");
    } else if (number == 1) {
        sprintf(tmp, "%s", "Ｂ");
    } else if (number == 2) {
        sprintf(tmp, "%s", "Ｃ");
    } else if (number == 3) {
        sprintf(tmp, "%s", "Ｄ");
    } else if (number == 4) {
        sprintf(tmp, "%s", "Ｅ");
    } else if (number == 5) {
        sprintf(tmp, "%s", "Ｆ");
    } else {
        sprintf(tmp, "%s", "Ｚ");
    }

    sprintf(alphabet_to_return, "%s", tmp);

    return 0;
}



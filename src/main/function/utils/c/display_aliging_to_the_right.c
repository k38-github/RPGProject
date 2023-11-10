#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../headers/display_aliging_to_the_right.h"
#include "../../utils/headers/display_character_string.h"
#include "../../utils/headers/u8mb.h"
#include "../../parts.h"

int display_aliging_to_the_right(SDL_Renderer *renderer, TTF_Font *font, char *string, double right_x, double right_y, int size) {
    // right_x, right_yは1番右にくる文字の右上の座標を指定する

    int word_counter = 0;
    int byt;
    int sum_byt = 0;

    int length;

    length = strlen(string);

    while (*string != '\0') {

        byt = u8mb(*string);
        sum_byt = sum_byt + byt;

        // printf("%dbyte文字です\n", byt);

        string += u8mb(*string);

        word_counter++;

    }

    string = string - sum_byt;

    // printf("%s: %d文字入力されました。\n", string, word_counter);
    // printf("合計 %dbyte入力されました。\n", sum_byt);

    right_x = right_x - (double)(size * word_counter);

    display_character_string(renderer, font, string, right_x, right_y,  1);

    return 0;
}

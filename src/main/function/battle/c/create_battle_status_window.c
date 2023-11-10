#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../headers/create_battle_status_window.h"
#include "../../utils/headers/convert_int_to_full_width_char.h"
#include "../../utils/headers/display_aliging_to_the_right.h"
#include "../../utils/headers/display_character_string.h"
#include "../../utils/headers/make_window.h"
#include "../../parts.h"

int create_battle_status_window(SDL_Renderer *renderer, TTF_Font *font) {

    char hp[10] = {0};
    char mp[10] = {0};
    char level[10] = {0};

    make_window(renderer, battle_status_window);

    display_character_string(renderer, font, player.status.name, 95.0, 30.0, 1);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 64, 55, 180, 55);

    display_character_string(renderer, font, "Ｈ", 74.0, 65.0, 1);
    convert_int_to_full_width_char(player.status.hp, hp);
    display_aliging_to_the_right(renderer, font, hp, 158.0, 65.0, 16);

    display_character_string(renderer, font, "Ｍ", 74.0, 95.0, 1);
    convert_int_to_full_width_char(player.status.mp, mp);
    display_aliging_to_the_right(renderer, font, mp, 158.0, 95.0, 16);

    display_character_string(renderer, font, "lv：", 74.0, 125.0, 1);
    convert_int_to_full_width_char(player.status.level, level);
    display_aliging_to_the_right(renderer, font, level, 158.0, 125.0, 16);


    return 0;
}



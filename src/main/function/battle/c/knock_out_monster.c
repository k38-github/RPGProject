#include <stdio.h>
#include "../headers/create_battle_status_window.h"
#include "../headers/knock_out_monster.h"
#include "../../draw/headers/draw_map.h"
#include "../../draw/headers/draw_monster.h"
#include "../../draw/headers/draw_treasure.h"
#include "../../draw/headers/draw_door.h"
#include "../../npc/headers/npc_animation.h"
#include "../../utils/headers/flush_message_engine.h"
#include "../../utils/headers/make_window.h"
#include "../../utils/headers/message_window_status.h"
#include "../../parts.h"

int knock_out_monster(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e, char *path, int num_of_monster,
                      MONSTER *monster_object, char *mes_buf) {

    SDL_RenderClear(renderer);

    draw_map(renderer);
    draw_treasure(renderer);
    draw_door(renderer, e);
    npc_animation(renderer);

    make_window(renderer, battle_back_window);
    draw_monster(renderer, path, num_of_monster, monster_object);

    create_battle_status_window(renderer, font);

    state = ON;
    message_window_status();

    message = mes_buf;
    make_window(renderer, message_window);
    flush_message_engine(renderer, font, e);

    SDL_RenderPresent(renderer);

    return 0;
}

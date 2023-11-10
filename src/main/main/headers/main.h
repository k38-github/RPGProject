#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../../function/parts.h"
#include "../../function/battle/headers/create_battle_status_window.h"
#include "../../function/battle/headers/battle_encount.h"
#include "../../function/battle/headers/battle_window.h"
#include "../../function/draw/headers/draw_door.h"
#include "../../function/draw/headers/draw_map.h"
#include "../../function/draw/headers/draw_monster.h"
#include "../../function/draw/headers/draw_treasure.h"
#include "../../function/effect/headers/effect.h"
#include "../../function/sounds/headers/sounds.h"
#include "../../function/load/headers/load.h"
#include "../../function/load/headers/load_monster.h"
#include "../../function/load/headers/load_monster_status.h"
#include "../../function/load/headers/load_move.h"
#include "../../function/load/headers/load_treasure.h"
#include "../../function/load/headers/load_door.h"
#include "../../function/load/headers/load_npc.h"
#include "../../function/npc/headers/npc_animation.h"
#include "../../function/npc/headers/npc_move.h"
#include "../../function/npc/headers/npc_update.h"
#include "../../function/player/headers/player_animation.h"
#include "../../function/player/headers/player_update.h"
#include "../../function/player/headers/player_move.h"
#include "../../function/utils/headers/calc_offset.h"
#include "../../function/utils/headers/convert_int_to_full_width_char.h"
#include "../../function/utils/headers/display_aliging_to_the_right.h"
#include "../../function/utils/headers/display_character_string.h"
#include "../../function/utils/headers/fade_out.h"
#include "../../function/utils/headers/get_npc_message.h"
#include "../../function/utils/headers/get_treasure_message.h"
#include "../../function/utils/headers/is_movable.h"
#include "../../function/utils/headers/make_triangle.h"
#include "../../function/utils/headers/make_window.h"
#include "../../function/utils/headers/open_door.h"
#include "../../function/utils/headers/window_update.h"
#include "../../function/utils/headers/message_engine.h"
#include "../../function/utils/headers/message_window_status.h"

/*
 * macro definition
 */

/*
 * type definition
 */

/*
 * prototype declaration
 */
int set_player_status();

int load_map_image(SDL_Renderer *, SDL_Texture **);

int make_box(SDL_Renderer *, int, int, int, int, int, int, int, int);

int draw_debug_info(SDL_Renderer *, TTF_Font *);

int make_commands_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int check_command_status(COMMAND_STATUS *, int, int);
int get_command_triangle(int *, int *, int *, int *, int *, int *);

int make_commands_back_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int make_status_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int check_status_status(STATUS_STATUS *, int, int);
int get_status_triangle(int *, int *, int *, int *, int *, int *);

int make_hp_and_mp_window(SDL_Renderer *, TTF_Font *, SDL_Event);


/*
 * global variable external reference declaration
 */
#endif

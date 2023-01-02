#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../../function/parts.h"
#include "../../function/effect/headers/effect.h"
#include "../../function/sounds/headers/sounds.h"
#include "../../function/load/headers/load.h"

/*
 * macro definition
 */

/*
 * type definition
 */

/*
 * prototype declaration
 */
int clac_offset(int, int, int *, int *);
int player_animation(SDL_Renderer *, SDL_Texture *);
int player_update(SDL_Renderer *, SDL_Event, SDL_Texture *);
int player_move(SDL_Event);
int set_player_status();

int load_npc(SDL_Renderer *);
int npc_animation(SDL_Renderer *);
int npc_update(SDL_Renderer *renderer, int);
int npc_move(DIRECTION, int);

int load_monster(SDL_Renderer *);
int load_monster_status(char *, int, int);

int battle_window(SDL_Renderer *, SDL_Event, MONSTER);
int battle_encount(SDL_Renderer *, SDL_Event);
int draw_monster(SDL_Renderer *, char *, int, MONSTER *);

int load_map_image(SDL_Renderer *, SDL_Texture **);
int load_move(SDL_Renderer *, SDL_Texture *);
int draw_map(SDL_Renderer *);
int load_treasure(SDL_Renderer *);
int draw_treasure(SDL_Renderer *);
int load_door(SDL_Renderer *);
int draw_door(SDL_Renderer *, SDL_Event);
int open_door(void);
int is_movable(int, int);
int fade_out(SDL_Renderer *, SDL_Texture *);

int make_window(SDL_Renderer *, WINDOW);
int make_box(SDL_Renderer *, int, int, int, int, int, int, int, int);
int make_triangle(SDL_Renderer *, int, int, int, int, int, int, int, int, int, int, int);
int window_update(SDL_Renderer *, TTF_Font *, SDL_Event);
int message_window_status();
int message_engine(SDL_Renderer *, TTF_Font *, SDL_Event);
int flush_message_engine(SDL_Renderer *, TTF_Font *, SDL_Event);
int display_character_string(SDL_Renderer *, TTF_Font *, char *, double, double, int);
int display_aliging_to_the_right(SDL_Renderer *, TTF_Font *, char *, double, double, int);
int get_message();
int get_treasure_message();
int get_npc_message();

int u8mb(const char);
int flash_triangle(SDL_Renderer *);

int draw_debug_info(SDL_Renderer *, TTF_Font *);

int make_commands_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int check_command_status(COMMAND_STATUS *, int, int);
int get_command_triangle(int *, int *, int *, int *, int *, int *);

int make_commands_back_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int make_status_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int check_status_status(STATUS_STATUS *, int, int);
int get_status_triangle(int *, int *, int *, int *, int *, int *);

int make_hp_and_mp_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int convert_int_to_full_width_char(int, char *);
int convert_int_to_alphabet(int, char *);

int knock_out_monster(SDL_Renderer *renderer, SDL_Event, char *, int, MONSTER *, char *);
int compare_agility(const void *, const void *);
int create_battle_status_window(SDL_Renderer *);

/*
 * global variable external reference declaration
 */
#endif

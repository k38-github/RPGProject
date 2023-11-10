#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "../headers/main.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int IMAGE_WIDTH = 16;
const int IMAGE_HEIGHT = 16;
const int MAGNIFICATION = 2;
const int GRID_SIZE = 32;
const int FONT_SIZE = 16;

int GOLD = 0;

int ROW = 15;
int COL = 20;
int OUT_OF_MAP = 0;

MAPCHIP mapchip[256] = {0};
char MAP_EVENT_NAME[256] = "field";
int map_array[65536] = {0};
int number_of_map_image = 0;

NPC npc[256] = {0};
int number_of_npc_image = 0;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *player_image = NULL;
TTF_Font *font = NULL;
TTF_Font *title_font = NULL;
TTF_Font *main_title_font = NULL;


int animecycle = 24;
int speed = 2;
int frame = 0;
int number_of_monster = 0;

CHARACTER player = {1, 1, 32, 32, 0, 0, 0, 0, DOWN, FALSE};
MONSTER monster[256] = {0};

TREASURE_FRAME treasure[256] = {0};
DOOR door[256] = {0};

WINDOW message_window = {140, 334, 360, 140, 255, OUT_VISIBLE};
WINDOW debug_window = {5, 5, 100, 50, 255, OUT_VISIBLE};
WINDOW command_window = {16, 16, 216, 160, 255, OUT_VISIBLE};
WINDOW status_window = {32, 32, 170, 135, 255, OUT_VISIBLE};
WINDOW player_status_window = {500, 320, 120, 140, 255, OUT_VISIBLE};
WINDOW hp_and_mp_window = {48, 48, 170, 220, 255, OUT_VISIBLE};
WINDOW gold_window = {480, 32, 140, 30, 255, OUT_VISIBLE};

WINDOW battle_back_window = {-10, 100, 660, 238, 255, OUT_VISIBLE};
WINDOW battle_status_window = {64, 20, 120, 140, 255, OUT_VISIBLE};
WINDOW battle_select_window = {64, 314, 140, 60, 255, OUT_VISIBLE};
WINDOW battle_action_window = {64, 290, 140, 105, 255, OUT_VISIBLE};
WINDOW battle_enemy_window = {204, 314, 380, 40, 255, OUT_VISIBLE};

char *message = SEARCH_MESSAGE;

STATE state = OFF;
STATE debug_state = OFF;
STATE flash_triangle_status = ON;
COMMAND_STATUS command_status = TALK;
STATUS_STATUS status_status = HP_AND_MP;


int main (int argc, char *argv[]) {

    // Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    window = SDL_CreateWindow( "CHIHA QUEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    } else {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    //

    // Initialize TTF
    if ( TTF_Init() < 0 ) {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
    }

    font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if ( font == NULL ) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }
    //

    // Initialze Mixer
    initialize_sounds();

    // Initialize Data

    load_mapchip(renderer, mapchip, number_of_map_image);
    load_map("src/resources/map/field.map", map_array, &COL, &ROW, &OUT_OF_MAP);

    load_image(renderer, &player_image, "src/resources/image/charachip/chiharu.bmp");
    set_player_status();

    load_npc(renderer);
    load_monster(renderer);

    int i, j;
    for (i = 0;i < sizeof(treasure)/sizeof(treasure[0]);i++) {
        strcpy(treasure[i].map, "empty");

        for (j = 0;j < sizeof(treasure->treasure)/sizeof(treasure->treasure[0]);j++) {
            strcpy(treasure[i].treasure[j].item, "empty");
        }
    }

    for (i = 0;i < sizeof(door)/sizeof(door[0]);i++) {
        door[i].status = 9;
    }

    //

    // title window
    main_title_font = TTF_OpenFont(FONT_PATH, FONT_SIZE*6);
    if ( main_title_font == NULL ) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }

    title_font = TTF_OpenFont(FONT_PATH, FONT_SIZE*2);
    if ( title_font == NULL ) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }

    int START_POSITION_X = (SCREEN_WIDTH / 5) * 2;
    int START_POSITION_Y = SCREEN_HEIGHT - 100;
    int OPTION_POSITION_X = (SCREEN_WIDTH / 5) * 2;
    int OPTION_POSITION_Y = SCREEN_HEIGHT - 68;

    int t_triangle_x1 = START_POSITION_X - 38;
    int t_triangle_y1 = SCREEN_HEIGHT - 96;
    int t_triangle_x2 = START_POSITION_X - 10;
    int t_triangle_y2 = SCREEN_HEIGHT - 82;
    int t_triangle_x3 = t_triangle_x1;
    int t_triangle_y3 = SCREEN_HEIGHT - 68;

    STATE start_flg = ON;

    while (1) {
        SDL_Event e;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect rectangle;

        rectangle.x = 0;
        rectangle.y = 0;
        rectangle.w = SCREEN_WIDTH;
        rectangle.h = SCREEN_HEIGHT;
        SDL_RenderFillRect(renderer, &rectangle);

        display_character_string(renderer, main_title_font, "CHIHARU QUEST", 10, 80,  1);
        display_character_string(renderer, title_font, "START", START_POSITION_X, SCREEN_HEIGHT - 100,  1);
        display_character_string(renderer, title_font, "OPTION", START_POSITION_X, SCREEN_HEIGHT - 68,  1);

        make_triangle(renderer, t_triangle_x1, t_triangle_y1,
                                t_triangle_x2, t_triangle_y2,
                                t_triangle_x3, t_triangle_y3,
                                255, 255, 255, 255, 1);

        SDL_RenderPresent(renderer);

        // event handling
        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP){
                t_triangle_y1 = SCREEN_HEIGHT - 96;
                t_triangle_y2 = SCREEN_HEIGHT - 82;
                t_triangle_y3 = SCREEN_HEIGHT - 68;

                start_flg = ON;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN){
                t_triangle_y1 = SCREEN_HEIGHT - 64;
                t_triangle_y2 = SCREEN_HEIGHT - 50;
                t_triangle_y3 = SCREEN_HEIGHT - 36;

                start_flg = OFF;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                if (start_flg == ON){
                    start_flg = OFF;
                    sprintf(MAP_EVENT_NAME, "%s", "field");
                    load_bgm(MAP_EVENT_NAME);
                    break;
                }
            }
        }
    }

    // main loop
    while (1) {
        SDL_Delay(5);
        SDL_Event e;

        calc_offset(player.pixel_x, player.pixel_y, &player.offset_x, &player.offset_y);

        SDL_RenderClear(renderer);
        draw_map(renderer);
        draw_treasure(renderer);
        draw_door(renderer, e);

        npc_animation(renderer);

        player_animation(renderer, player_image);
        player_update(renderer, e, player_image);

        if (debug_state == ON) {
            draw_debug_info(renderer, font);
        }

        window_update(renderer, font, e);

        SDL_RenderPresent(renderer);

        // event handling
        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_d){
                if (debug_state == OFF) {
                    debug_state = ON;
                } else {
                    debug_state = OFF;
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                message_window_status();
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN){
                make_commands_window(renderer, font, e);
            }
        }

    }

    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    for (i = 0;i < number_of_map_image;i++) {
        SDL_DestroyTexture(mapchip[i].map_image);
    }

    for (i = 0;i < number_of_npc_image;i++) {
        SDL_DestroyTexture(npc[i].npc_image);
    }

    SDL_DestroyTexture(player_image);
    SDL_Quit();

    quit_sounds();

    return 0;

}

int set_player_status() {
    strcpy(player.status.name, "ちはる");
    player.status.level = 1;
    player.status.hp = 35;
    player.status.mp = 20;
    player.status.max_hp = 35;
    player.status.max_mp = 20;
    player.status.strength = 15;
    player.status.agility = 3;
    player.status.protection = 10;

    return 0;
}

int make_box(SDL_Renderer *renderer, int x, int y, int w, int h, int blend, int r, int g, int b) {

    SDL_Rect rectangle;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    rectangle.x = x;
    rectangle.y = y;
    rectangle.w = w;
    rectangle.h = h;

    SDL_SetRenderDrawColor(renderer, r, g, b, blend);

    SDL_RenderFillRect(renderer, &rectangle);

    return 0;
}

int draw_debug_info(SDL_Renderer *renderer, TTF_Font *font) {

    char coordinate[10];

    make_window(renderer, debug_window);

    sprintf(coordinate, "%03d %03d", player.map_x, player.map_y);
    display_character_string(renderer, font, MAP_EVENT_NAME, 10.0, 10.0, 0);
    display_character_string(renderer, font, coordinate, 10.0, 30.0, 0);

    return 0;

}

int make_commands_back_window(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e) {

    int triangle_x1 = 30;
    int triangle_y1 = 37;
    int triangle_x2 = 45;
    int triangle_y2 = 46;
    int triangle_x3 = 30;
    int triangle_y3 = 56;

    get_command_triangle(&triangle_x1, &triangle_y1,
                         &triangle_x2, &triangle_y2,
                         &triangle_x3, &triangle_y3);

    make_window(renderer, command_window);
    make_triangle(renderer, triangle_x1, triangle_y1,
                            triangle_x2, triangle_y2,
                            triangle_x3, triangle_y3,
                            255, 255, 255, 255, 1);

    display_character_string(renderer, font, "はなす",   50.0, 35.0,  1);
    display_character_string(renderer, font, "おぼえる", 50.0, 60.0,  1);
    display_character_string(renderer, font, "つよさ",   50.0, 85.0,  1);
    display_character_string(renderer, font, "そうび",   50.0, 110.0, 1);
    display_character_string(renderer, font, "とびら",   50.0, 135.0, 1);

    display_character_string(renderer, font, "じゅもん", 150.0, 35.0,  1);
    display_character_string(renderer, font, "スキル",   150.0, 60.0,  1);
    display_character_string(renderer, font, "どうぐ",   150.0, 85.0,  1);
    display_character_string(renderer, font, "さくせん", 150.0, 110.0, 1);
    display_character_string(renderer, font, "しらべる", 150.0, 135.0, 1);

    char hp[10] = {0};
    char mp[10] = {0};
    char level[10] = {0};

    make_window(renderer, player_status_window);

    display_character_string(renderer, font, player.status.name, 531.0, 330.0, 1);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 500, 355, 616, 355);

    display_character_string(renderer, font, "Ｈ", 510.0, 365.0, 1);
    convert_int_to_full_width_char(player.status.hp, hp);
    display_aliging_to_the_right(renderer, font, hp, 594.0, 365.0, 16);

    display_character_string(renderer, font, "Ｍ", 510.0, 395.0, 1);
    convert_int_to_full_width_char(player.status.mp, mp);
    display_aliging_to_the_right(renderer, font, mp, 594.0, 395.0, 16);

    display_character_string(renderer, font, "lv：", 510.0, 425.0, 1);
    convert_int_to_full_width_char(player.status.level, level);
    display_aliging_to_the_right(renderer, font, level, 594.0, 425.0, 16);


    SDL_RenderPresent(renderer);

    return 0;
}

int make_commands_window(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e) {

    int triangle_x1 = 30;
    int triangle_y1 = 37;
    int triangle_x2 = 45;
    int triangle_y2 = 46;
    int triangle_x3 = 30;
    int triangle_y3 = 56;

    get_command_triangle(&triangle_x1, &triangle_y1,
                         &triangle_x2, &triangle_y2,
                         &triangle_x3, &triangle_y3);

    make_window(renderer, command_window);
    make_triangle(renderer, triangle_x1, triangle_y1,
                            triangle_x2, triangle_y2,
                            triangle_x3, triangle_y3,
                            255, 255, 255, 255, 1);

    display_character_string(renderer, font, "はなす",   50.0, 35.0,  1);
    display_character_string(renderer, font, "おぼえる", 50.0, 60.0,  1);
    display_character_string(renderer, font, "つよさ",   50.0, 85.0,  1);
    display_character_string(renderer, font, "そうび",   50.0, 110.0, 1);
    display_character_string(renderer, font, "とびら",   50.0, 135.0, 1);

    display_character_string(renderer, font, "じゅもん", 150.0, 35.0,  1);
    display_character_string(renderer, font, "スキル",   150.0, 60.0,  1);
    display_character_string(renderer, font, "どうぐ",   150.0, 85.0,  1);
    display_character_string(renderer, font, "さくせん", 150.0, 110.0, 1);
    display_character_string(renderer, font, "しらべる", 150.0, 135.0, 1);

    char hp[10] = {0};
    char mp[10] = {0};
    char level[10] = {0};

    make_window(renderer, player_status_window);

    display_character_string(renderer, font, player.status.name, 531.0, 330.0, 1);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 500, 355, 616, 355);

    display_character_string(renderer, font, "Ｈ", 510.0, 365.0, 1);
    convert_int_to_full_width_char(player.status.hp, hp);
    display_aliging_to_the_right(renderer, font, hp, 594.0, 365.0, 16);

    display_character_string(renderer, font, "Ｍ", 510.0, 395.0, 1);
    convert_int_to_full_width_char(player.status.mp, mp);
    display_aliging_to_the_right(renderer, font, mp, 594.0, 395.0, 16);

    display_character_string(renderer, font, "lv：", 510.0, 425.0, 1);
    convert_int_to_full_width_char(player.status.level, level);
    display_aliging_to_the_right(renderer, font, level, 594.0, 425.0, 16);


    SDL_RenderPresent(renderer);

    char *se_file = "pi.ogg";
    while (1) {

        check_command_status(&command_status, triangle_x1, triangle_y1);
        SDL_RenderPresent(renderer);

        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
                sound_se(se_file);
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                sound_se(se_file);

                if (command_status == TALK) {
                    state = ON;
                    message_window_status();
                    get_npc_message();
                    break;
                } else if (command_status == MEMORY) {
                } else if (command_status == STATUS) {
                    make_status_window(renderer, font, e);
                    break;
                } else if (command_status == EQUIPMENT) {
                } else if (command_status == OPEN) {
                    state = ON;
                    open_door();
                    break;
                } else if (command_status == SPELL) {
                } else if (command_status == SKILL) {
                } else if (command_status == TOOLS) {
                } else if (command_status == TACTICS) {
                } else if (command_status == SEARCH) {
                    state = ON;
                    message_window_status();
                    get_treasure_message();
                    break;
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT){
                sound_se(se_file);
                if (triangle_x1 == 30) {
                    triangle_x1 = triangle_x1 + 100;
                    triangle_x2 = triangle_x2 + 100;
                    triangle_x3 = triangle_x3 + 100;
                }

                make_box(renderer, 30, 36, 16, 135, 255, 0, 0, 0);
                make_box(renderer, 130, 36, 16, 135, 255, 0, 0, 0);
                make_triangle(renderer, triangle_x1, triangle_y1,
                                            triangle_x2, triangle_y2,
                                            triangle_x3, triangle_y3,
                                            255, 255, 255, 255, 1);

            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT){
                sound_se(se_file);
                if (triangle_x1 == 130) {
                    triangle_x1 = triangle_x1 - 100;
                    triangle_x2 = triangle_x2 - 100;
                    triangle_x3 = triangle_x3 - 100;
                }

                make_box(renderer, 30, 36, 16, 135, 255, 0, 0, 0);
                make_box(renderer, 130, 36, 16, 135, 255, 0, 0, 0);
                make_triangle(renderer, triangle_x1, triangle_y1,
                                            triangle_x2, triangle_y2,
                                            triangle_x3, triangle_y3,
                                            255, 255, 255, 255, 1);

            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP){
                sound_se(se_file);
                if (triangle_y1 > 37) {
                    triangle_y1 = triangle_y1 - 25;
                    triangle_y2 = triangle_y2 - 25;
                    triangle_y3 = triangle_y3 - 25;
                }

                make_box(renderer, 30, 36, 16, 135, 255, 0, 0, 0);
                make_box(renderer, 130, 36, 16, 135, 255, 0, 0, 0);
                make_triangle(renderer, triangle_x1, triangle_y1,
                                            triangle_x2, triangle_y2,
                                            triangle_x3, triangle_y3,
                                            255, 255, 255, 255, 1);

            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN){
                sound_se(se_file);
                if (triangle_y3 <= 135) {
                    triangle_y1 = triangle_y1 + 25;
                    triangle_y2 = triangle_y2 + 25;
                    triangle_y3 = triangle_y3 + 25;
                }

                make_box(renderer, 30, 36, 16, 135, 255, 0, 0, 0);
                make_box(renderer, 130, 36, 16, 135, 255, 0, 0, 0);
                make_triangle(renderer, triangle_x1, triangle_y1,
                                            triangle_x2, triangle_y2,
                                            triangle_x3, triangle_y3,
                                            255, 255, 255, 255, 1);


            }
        }
    }

    return 0;
}

int check_command_status(COMMAND_STATUS *command_status, int triangle_x1, int triangle_y1) {

    if (triangle_x1 == 30) {
        if (triangle_y1 == 37) {
            *command_status = TALK;
        } else if (triangle_y1 == 62) {
            *command_status = MEMORY;
        } else if (triangle_y1 == 87) {
            *command_status = STATUS;
        } else if (triangle_y1 == 112) {
            *command_status = EQUIPMENT;
        } else if (triangle_y1 == 137) {
            *command_status = OPEN;
        }
    } else {
        if (triangle_y1 == 37) {
            *command_status = SPELL;
        } else if (triangle_y1 == 62) {
            *command_status = SKILL;
        } else if (triangle_y1 == 87) {
            *command_status = TOOLS;
        } else if (triangle_y1 == 112) {
            *command_status = TACTICS;
        } else if (triangle_y1 == 137) {
            *command_status = SEARCH;
        }
    }

    return 0;
}

int get_command_triangle(int *triangle_x1, int *triangle_y1,
                         int *triangle_x2, int *triangle_y2,
                         int *triangle_x3, int *triangle_y3) {
    if (command_status == TALK) {
        *triangle_x1 = 30;
        *triangle_y1 = 37;
        *triangle_x2 = 45;
        *triangle_y2 = 46;
        *triangle_x3 = 30;
        *triangle_y3 = 56;
    } else if (command_status == STATUS) {
        *triangle_x1 = 30;
        *triangle_y1 = 87;
        *triangle_x2 = 45;
        *triangle_y2 = 96;
        *triangle_x3 = 30;
        *triangle_y3 = 106;
    }

    return 0;
}

int make_status_window(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e) {

    int triangle_x1 = 47;
    int triangle_y1 = 80;
    int triangle_x2 = 62;
    int triangle_y2 = 89;
    int triangle_x3 = 47;
    int triangle_y3 = 99;

    char gold_full_width[20] = {0};

    get_status_triangle(&triangle_x1, &triangle_y1,
                        &triangle_x2, &triangle_y2,
                        &triangle_x3, &triangle_y3);


    make_window(renderer, status_window);
    make_triangle(renderer, triangle_x1, triangle_y1,
                            triangle_x2, triangle_y2,
                            triangle_x3, triangle_y3,
                            255, 255, 255, 255, 1);

    // x方向に+16
    make_window(renderer, gold_window);
    display_character_string(renderer, font, "Ｇ",   488.0, 38.0,  1);
    convert_int_to_full_width_char(GOLD, gold_full_width);
    display_aliging_to_the_right(renderer, font, gold_full_width, 616.0, 38.0, 16);

    display_character_string(renderer, font, "つよさ",   70.0, 50.0,  1);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 70, 73, 180, 73);
    display_character_string(renderer, font, "ＨＰとＭＰ",   70.0, 80.0,  1);
    display_character_string(renderer, font, "こうげきりょく",   70.0, 105.0, 1);
    display_character_string(renderer, font, "つよさをみる",   70.0, 130.0, 1);

    char *se_file = "pi.ogg";
    while (1) {

        check_status_status(&status_status, triangle_x1, triangle_y1);
        SDL_RenderPresent(renderer);

        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
                sound_se(se_file);
                SDL_RenderClear(renderer);
                draw_map(renderer);
                draw_treasure(renderer);
                draw_door(renderer, e);

                npc_animation(renderer);

                player_animation(renderer, player_image);

                make_commands_window(renderer, font, e);
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                sound_se(se_file);
                if (status_status == HP_AND_MP) {
                    make_hp_and_mp_window(renderer, font, e);

                    make_commands_back_window(renderer, font, e);
                    make_status_window(renderer, font, e);
                    break;
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP){
                sound_se(se_file);
                if (triangle_y1 > 80) {
                    triangle_y1 = triangle_y1 - 25;
                    triangle_y2 = triangle_y2 - 25;
                    triangle_y3 = triangle_y3 - 25;
                }

                make_box(renderer, 47, 80, 16, 75, 255, 0, 0, 0);
                make_triangle(renderer, triangle_x1, triangle_y1,
                                            triangle_x2, triangle_y2,
                                            triangle_x3, triangle_y3,
                                            255, 255, 255, 255, 1);

            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN){
                sound_se(se_file);
                if (triangle_y3 <= 135) {
                    triangle_y1 = triangle_y1 + 25;
                    triangle_y2 = triangle_y2 + 25;
                    triangle_y3 = triangle_y3 + 25;
                }

                make_box(renderer, 47, 80, 16, 75, 255, 0, 0, 0);
                make_triangle(renderer, triangle_x1, triangle_y1,
                                            triangle_x2, triangle_y2,
                                            triangle_x3, triangle_y3,
                                            255, 255, 255, 255, 1);
            }

        }
    } 

    return 0;
}

int check_status_status(STATUS_STATUS *status_status, int triangle_x1, int triangle_y1) {

    if (triangle_y1 == 80) {
        *status_status = HP_AND_MP;
    } else if (triangle_y1 == 105) {
        *status_status = OFFENSIVE_POWER;
    } else if (triangle_y1 == 130) {
        *status_status = CONFIRM_STATUS;
    }
    return 0;
}

int get_status_triangle(int *triangle_x1, int *triangle_y1,
                         int *triangle_x2, int *triangle_y2,
                         int *triangle_x3, int *triangle_y3) {
    if (command_status == HP_AND_MP) {
        *triangle_x1 = 47;
        *triangle_y1 = 80;
        *triangle_x2 = 62;
        *triangle_y2 = 89;
        *triangle_x3 = 47;
        *triangle_y3 = 99;
    }

    return 0;
}

int make_hp_and_mp_window(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e) {

    char level[10] = {0};
    char hp[10] = {0};
    char mp[10] = {0};
    char max_hp[10] = {0};
    char max_mp[10] = {0};

    make_window(renderer, hp_and_mp_window);


    display_character_string(renderer, font, "ＨＰとＭＰ",             70.0, 60.0,  1);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 70, 83, 200, 83);

    display_character_string(renderer, font, "レベル：",           70.0, 90.0,  1);
    convert_int_to_full_width_char(player.status.level, level);
    display_character_string(renderer, font, level,                135.0, 90.0,  1);

    display_character_string(renderer, font, "なまえ：",           70.0, 115.0, 1);
    display_character_string(renderer, font, player.status.name,   135.0, 115.0, 1);

    display_character_string(renderer, font, "Ｈ",                  70.0, 140.0, 1);
    convert_int_to_full_width_char(player.status.hp, hp);
    display_aliging_to_the_right(renderer, font, hp,               158.0, 140.0, 16);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 90, 163, 170, 163);
    convert_int_to_full_width_char(player.status.max_hp, max_hp);
    display_aliging_to_the_right(renderer, font, max_hp,           158.0, 170.0, 16);

    display_character_string(renderer, font, "Ｍ",                  70.0, 195.0, 1);
    convert_int_to_full_width_char(player.status.mp, mp);
    display_aliging_to_the_right(renderer, font, mp,               158.0, 195.0, 16);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 90, 218, 170, 218);
    convert_int_to_full_width_char(player.status.max_mp, max_mp);
    display_aliging_to_the_right(renderer, font, max_mp,           158.0, 225.0, 16);

    char *se_file = "pi.ogg";
    while (1) {

        SDL_RenderPresent(renderer);

        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
                sound_se(se_file);

                SDL_RenderClear(renderer);
                draw_map(renderer);
                draw_treasure(renderer);
                draw_door(renderer, e);

                npc_animation(renderer);

                player_animation(renderer, player_image);
                break;
            }
        }
    } 

    return 0;
}

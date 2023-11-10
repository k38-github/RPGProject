#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../headers/battle_window.h"
#include "../headers/compare_agility.h"
#include "../headers/create_battle_status_window.h"
#include "../headers/knock_out_monster.h"
#include "../../draw/headers/draw_door.h"
#include "../../draw/headers/draw_map.h"
#include "../../draw/headers/draw_monster.h"
#include "../../draw/headers/draw_treasure.h"
#include "../../effect/headers/effect.h"
#include "../../npc/headers/npc_animation.h"
#include "../../sounds/headers/sounds.h"
#include "../../utils/headers/convert_int_to_alphabet.h"
#include "../../utils/headers/convert_int_to_full_width_char.h"
#include "../../utils/headers/display_character_string.h"
#include "../../utils/headers/make_triangle.h"
#include "../../utils/headers/make_window.h"
#include "../../utils/headers/message_engine.h"
#include "../../utils/headers/message_window_status.h"
#include "../../utils/headers/window_update.h"
#include "../../parts.h"

int battle_window(SDL_Renderer *renderer, SDL_Event e, MONSTER monster) {

    int i;

    const double start_x = 153.0;
    const double start_y = 354.0;
    char path[300] = {0};
    char monster_name[300] = {0};
    char map_event_name_save[256] = {0};

    int triangle_x1 = 74;
    int triangle_y1 = 324;
    int triangle_x2 = 89;
    int triangle_y2 = 334;
    int triangle_x3 = 74;
    int triangle_y3 = 344;

    double x_pos = 240.0;
    double y_pos = 324.0;

    int battle = triangle_y1;
    int escape = triangle_y1 + 21;

    int battle_select = triangle_y1;
    int attack = triangle_y1;
    int defense = triangle_y1 + 21;
    int item = triangle_y1 + 42;

    char mes_buf[1000] = {0};
    char monster_buf[10] = {0};

    TTF_Font *font = NULL;
    font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if ( font == NULL ) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }
    //


    BATTLE_STATUS battle_status = NORMAL;

    sprintf(monster_name, "%s", monster.status.name);
    printf("monster name:%s\n", monster_name);
    sprintf(path, "src/resources/image/monster/%s.bmp", monster.status.name);
    sprintf(map_event_name_save, "%s", MAP_EVENT_NAME);
    sprintf(MAP_EVENT_NAME, "%s", "battle");
    load_bgm(MAP_EVENT_NAME);

    make_window(renderer, battle_back_window);
    make_window(renderer, message_window);

    sprintf(mes_buf, "%s%s", monster_name, ENCOUNT_MESSAGE);
    display_character_string(renderer, font, mes_buf, start_x, start_y,  1);

    srand((unsigned)time(NULL));
    int num_of_monster;
    if (rand()%100+1 < 90) {
        num_of_monster = rand()%3 + 1;
    } else {
        num_of_monster = rand()%5 + 1;
    }

    printf("num_of_monster: %d\n", num_of_monster);

    int rectangle_h = battle_enemy_window.rectangle_h;
    battle_enemy_window.rectangle_h = battle_enemy_window.rectangle_h + ((num_of_monster - 1) * 21);

    MONSTER *monster_object;
    monster_object = malloc(sizeof(MONSTER) * num_of_monster);

    for (i=0;i<num_of_monster;i++) {
        monster_object[i] = monster;
        convert_int_to_alphabet(i, monster_buf);
        strcat(monster_object[i].status.name, monster_buf);

        monster_object[i].status.agility = (rand()%2)+monster.status.agility;

        printf("monster %d: %s %d\n", i, monster_object[i].status.name, monster_object[i].status.agility);
    }

    draw_monster(renderer, path, num_of_monster, monster_object);

    SDL_RenderPresent(renderer);

    SDL_Delay(1500);

    while (1) {

        SDL_RenderClear(renderer);

        draw_map(renderer);
        draw_treasure(renderer);
        draw_door(renderer, e);
        npc_animation(renderer);

        make_window(renderer, battle_back_window);
        draw_monster(renderer, path, num_of_monster, monster_object);

        create_battle_status_window(renderer, font);

        if (battle_status == NORMAL) {

            make_window(renderer, battle_select_window);
            make_window(renderer, battle_enemy_window);

            display_character_string(renderer, font, "たたかう", 100.0, 324.0, 1);
            display_character_string(renderer, font, "にげる",   100.0, 345.0, 1);

            x_pos = 240.0;
            y_pos = 324.0;

            for(i=0;i<num_of_monster;i++){
                if(0 < monster_object[i].status.hp) {
                    display_character_string(renderer, font, monster_object[i].status.name, x_pos, y_pos, 1);

                    y_pos = y_pos + 21;
                }
            }


            make_triangle(renderer, triangle_x1, triangle_y1,
                                    triangle_x2, triangle_y2,
                                    triangle_x3, triangle_y3,
                                    255, 255, 255, 255, 1);

        } else if (battle_status == BATTLE_ACTION) {
            make_window(renderer, battle_action_window);
            make_window(renderer, battle_enemy_window);

            display_character_string(renderer, font, player.status.name, 100.0, 298.0, 1);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(renderer, 64, 318, 200, 318);


            display_character_string(renderer, font, "こうげき", 100.0, 324.0, 1);
            display_character_string(renderer, font, "ぼうぎょ", 100.0, 345.0, 1);
            display_character_string(renderer, font, "どうぐ",   100.0, 366.0, 1);

            x_pos = 240.0;
            y_pos = 324.0;

            for(i=0;i<num_of_monster;i++){
                if(0 < monster_object[i].status.hp) {
                    display_character_string(renderer, font, monster_object[i].status.name, x_pos, y_pos, 1);

                    y_pos = y_pos + 21;
                }
            }

            make_triangle(renderer, triangle_x1, triangle_y1,
                                    triangle_x2, triangle_y2,
                                    triangle_x3, triangle_y3,
                                    255, 255, 255, 255, 1);

        } else if (battle_status == BATTLE_SELECT) {
            make_window(renderer, battle_action_window);
            make_window(renderer, battle_enemy_window);

            display_character_string(renderer, font, player.status.name, 100.0, 298.0, 1);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(renderer, 64, 318, 200, 318);


            display_character_string(renderer, font, "こうげき", 100.0, 324.0, 1);
            display_character_string(renderer, font, "ぼうぎょ", 100.0, 345.0, 1);
            display_character_string(renderer, font, "どうぐ",   100.0, 366.0, 1);

            x_pos = 240.0;
            y_pos = 324.0;

            for(i=0;i<num_of_monster;i++){
                if(0 < monster_object[i].status.hp) {
                    display_character_string(renderer, font, monster_object[i].status.name, x_pos, y_pos, 1);

                    y_pos = y_pos + 21;
                }
            }

            make_triangle(renderer, triangle_x1+140, triangle_y1,
                                    triangle_x2+140, triangle_y2,
                                    triangle_x3+140, triangle_y3,
                                    255, 255, 255, 255, 1);

        } else if (battle_status == ATTACK) {
            int j = 0;
            int order[5] = {0};

            ENEMY_POS enemy_pos = ENEMY_POS_1;

            flash_triangle_status = OFF;

            printf("attack:%d\n", attack);

            for(i=0;i<num_of_monster;i++){
                printf("%s %d\n",monster_object[i].status.name, monster_object[i].status.hp);
                if(0 < monster_object[i].status.hp) {
                    order[j] = i;
                    j++;
                }
            }

            if (attack == 324) {
                enemy_pos = ENEMY_POS_1;
            } else if (attack == 345) {
                enemy_pos = ENEMY_POS_2;
            } else if (attack == 366) {
                enemy_pos = ENEMY_POS_3;
            } else if (attack == 387) {
                enemy_pos = ENEMY_POS_4;
            } else if (attack == 408) {
                enemy_pos = ENEMY_POS_5;
            }

            // decide the attack order
            MONSTER *monster_sort;
            monster_sort = malloc(sizeof(MONSTER) * j);

            for (i=0;i<j;i++) {
                monster_sort[i] = monster_object[order[i]];
                printf("mo_so: %s\n", monster_sort[i].status.name);
            }

            qsort(monster_sort, sizeof(monster_sort) / sizeof(monster_sort[0]), sizeof(MONSTER), compare_agility);

            int player_damage;
            int monster_damage;
            char damage_hull_width[30] = {0};

            srand((unsigned)time(NULL));
            monster_damage = (player.status.strength - (monster_object[order[enemy_pos]].status.protection / 2)) *
                             (99 + rand()%55) / 256;
            if (monster_damage < 0) {
                monster_damage = 0;
            }

            srand((unsigned)time(NULL));
            player_damage = (monster_object[order[enemy_pos]].status.strength - (player.status.protection / 2)) *
                             (99 + rand()%55) / 256;
            if (player_damage < 0) {
                player_damage = 0;
            }

            printf("monster_damage: %d player_damage: %d\n", monster_damage, player_damage);

            STATE player_attack = OFF;
            for (i=0;i<j;i++) {
                printf("sort: %s %d\n", monster_sort[i].status.name, monster_sort[i].status.agility); 
                if (player_attack == OFF && monster_sort[i].status.agility <= player.status.agility) {
                    player_attack = ON;
                    strcpy(mes_buf, "\0");

                    state = ON;
                    message_window_status();

                    monster_object[order[enemy_pos]].status.hp = monster_object[order[enemy_pos]].status.hp - monster_damage;

                    sprintf(mes_buf,"%sの　こうげき！", player.status.name);

                    message = mes_buf;
                    window_update(renderer, font, e);

                    blow_effects(renderer,
                                 monster_object[order[enemy_pos]].imageRect.w,
                                 monster_object[order[enemy_pos]].imageRect.h,
                                 monster_object[order[enemy_pos]].drawRect.x,
                                 monster_object[order[enemy_pos]].drawRect.y);

                    draw_monster(renderer, path, num_of_monster, monster_object);

                    damage_flush(renderer,
                                 &monster_object[order[enemy_pos]].monster_image,
                                 monster_object[order[enemy_pos]].imageRect,
                                 monster_object[order[enemy_pos]].drawRect);


                    convert_int_to_full_width_char(monster_damage, damage_hull_width);

                    strcpy(mes_buf, "\0");
                    sprintf(mes_buf,"  %sに　%sの　ダメージ！！", monster_object[order[enemy_pos]].status.name, damage_hull_width);

                    message = mes_buf;
                    message_engine(renderer, font, e);

                    if (monster_object[order[enemy_pos]].status.hp <= 0) {
                        int k;
                        for (k=0;k<j;k++) {
                            if (strcmp(monster_sort[k].status.name,
                                       monster_object[order[enemy_pos]].status.name) == 0) {
                                monster_sort[k].status.hp = 0;
                            }
                        }

                        strcpy(mes_buf, "\0");
                        sprintf(mes_buf,"%sの　こうげき！  %sに　%sの　ダメージ！！",
                                      player.status.name, monster_object[order[enemy_pos]].status.name, damage_hull_width);


                        knock_out_monster(renderer, font, e, path, num_of_monster, monster_object, mes_buf);
                        SDL_Delay(400);

                        strcpy(mes_buf, "\0");
                        sprintf(mes_buf, "    %sを　たおした！", monster_object[order[enemy_pos]].status.name);
                        battle_enemy_window.rectangle_h = battle_enemy_window.rectangle_h - 21;

                        message = mes_buf;
                        message_engine(renderer, font, e);
                    }

                    i--;
                } else {
                    if (0 < monster_sort[i].status.hp) {
                        state = ON;
                        message_window_status();

                        convert_int_to_full_width_char(player_damage, damage_hull_width);
                        sprintf(mes_buf,"%sの　こうげき！  %sに　%sの　ダメージ！！",
                                        monster_sort[i].status.name, player.status.name,  damage_hull_width);

                        player.status.hp = player.status.hp - player_damage;

                        message = mes_buf;
                        window_update(renderer, font, e);

                        create_battle_status_window(renderer, font);
                    }

                }
            }

            if (player_attack == OFF) {
                strcpy(mes_buf, "\0");

                state = ON;
                message_window_status();

                monster_object[order[enemy_pos]].status.hp = monster_object[order[enemy_pos]].status.hp - monster_damage;

                sprintf(mes_buf,"%sの　こうげき！", player.status.name);

                message = mes_buf;
                window_update(renderer, font, e);
                blow_effects(renderer,
                             monster_object[order[enemy_pos]].imageRect.w,
                             monster_object[order[enemy_pos]].imageRect.h,
                             monster_object[order[enemy_pos]].drawRect.x,
                             monster_object[order[enemy_pos]].drawRect.y);

                draw_monster(renderer, path, num_of_monster, monster_object);

                damage_flush(renderer,
                             &monster_object[order[enemy_pos]].monster_image,
                             monster_object[order[enemy_pos]].imageRect,
                             monster_object[order[enemy_pos]].drawRect);

                convert_int_to_full_width_char(monster_damage, damage_hull_width);

                strcpy(mes_buf, "\0");
                sprintf(mes_buf,"  %sに　%sの　ダメージ！！", monster_object[order[enemy_pos]].status.name, damage_hull_width);

                message = mes_buf;
                message_engine(renderer, font, e);


                if (monster_object[order[enemy_pos]].status.hp <= 0) {
                    strcpy(mes_buf, "\0");
                    sprintf(mes_buf,"%sの　こうげき！  %sに　%sの　ダメージ！！",
                                  player.status.name, monster_object[order[enemy_pos]].status.name, damage_hull_width);

                    knock_out_monster(renderer, font, e, path, num_of_monster, monster_object, mes_buf);
                    SDL_Delay(400);

                    strcpy(mes_buf, "\0");
                    sprintf(mes_buf, "    %sを　たおした！", monster_object[order[enemy_pos]].status.name);
                    battle_enemy_window.rectangle_h = battle_enemy_window.rectangle_h - 21;

                    message = mes_buf;
                    message_engine(renderer, font, e);
                }
            }


            free(monster_sort);

            for(i=0;i<num_of_monster;i++){
                if(0 < monster_object[i].status.hp) {
                    battle_status = NORMAL;
                }
            }

            if (battle_status == ATTACK) {
                battle_status = BATTLE_END;
            }

            flash_triangle_status = ON;

            triangle_x1 = 74;
            triangle_y1 = 324;
            triangle_x2 = 89;
            triangle_y2 = 334;
            triangle_x3 = 74;
            triangle_y3 = 344;

        } else if (battle_status == DEFENSE) {
            flash_triangle_status = OFF;

            strcpy(mes_buf, "\0");

            state = ON;
            message_window_status();
            player.status.protection = player.status.protection * 1.4;
            printf("protection: %d\n", player.status.protection);

            sprintf(mes_buf, "%sは　みをまもっている！", player.status.name);
            message = mes_buf;
            window_update(renderer, font, e);

            int player_damage;
            char damage_hull_width[30] = {0};

            for(i=0;i<num_of_monster;i++){
                if (0 < monster_object[i].status.hp) {
                    state = ON;
                    message_window_status();

                    srand((unsigned)time(NULL));
                    player_damage = (monster_object[i].status.strength - (player.status.protection / 2)) *
                                     (99 + rand()%55) / 256;
                    if (player_damage < 0) {
                        player_damage = 0;
                    }

                    convert_int_to_full_width_char(player_damage, damage_hull_width);
                    sprintf(mes_buf,"%sの　こうげき！  %sに　%sの　ダメージ！！", 
                                    monster_object[i].status.name, player.status.name,  damage_hull_width);

                    player.status.hp = player.status.hp - player_damage;

                    message = mes_buf;
                    window_update(renderer, font, e);

                    create_battle_status_window(renderer, font);
                }
            }

            battle_status = NORMAL;

            player.status.protection = player.status.protection / 1.4;

            triangle_y1 = triangle_y1 - 21;
            triangle_y2 = triangle_y2 - 21;
            triangle_y3 = triangle_y3 - 21;

        } else if (battle_status == ESCAPE) {
            strcpy(mes_buf, "\0");

            state = ON;
            message_window_status();

            if (monster.status.agility + num_of_monster/10 < player.status.agility) {
                sprintf(mes_buf, "%sから　にげきれた！", monster_name);
                message = mes_buf;
                window_update(renderer, font, e);
                break;
            } else {
                srand((unsigned)time(NULL));
                int escape = rand()%(abs(monster.status.agility - player.status.agility) + 3);
                printf("escape:%d %d\n", monster.status.agility, escape + player.status.agility);
 
                if ( monster.status.agility + num_of_monster/10 < escape + player.status.agility) {
                    sprintf(mes_buf, "%sから　にげきれた！", monster_name);
                    message = mes_buf;
                    window_update(renderer, font, e);
                    break;
                } else {
                    sprintf(mes_buf, "%sは　とうそうした!  しかし、まわりこまれてしまった。", player.status.name);
                    message = mes_buf;
                    window_update(renderer, font, e);
                    battle_status = NORMAL;
                }
            }
        } else if (battle_status == BATTLE_END) {
            printf("battle_end\n");
            char gold_hull_width[30] = {0};
            char experience_hull_width[30] = {0};

            pause_sounds();
            sound_se("battle_end.ogg");

            strcpy(mes_buf, "\0");

            state = ON;
            message_window_status();

            GOLD = GOLD + monster.gold * num_of_monster;
            player.status.experience_point = player.status.experience_point + monster.status.experience_point;

            convert_int_to_full_width_char(monster.gold, gold_hull_width);
            convert_int_to_full_width_char(monster.status.experience_point, experience_hull_width);
            sprintf(mes_buf, "%sを　たおした！**%sポイントの　けいけんちを　かくとく！  %sゴールドを　てにいれた！",
                              monster.status.name, experience_hull_width, gold_hull_width);
            message = mes_buf;

            window_update(renderer, font, e);

            break;
        }

        SDL_RenderPresent(renderer);

        // event handling
        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
                triangle_x1 = 74;
                triangle_y1 = 324;
                triangle_x2 = 89;
                triangle_y2 = 334;
                triangle_x3 = 74;
                triangle_y3 = 344;

                if (battle_status == BATTLE_SELECT) {
                    battle_status = BATTLE_ACTION;
                } else if (battle_status == BATTLE_ACTION) {
                    battle_status = NORMAL;
                } else {
                    break;
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP){
                if (triangle_y1 > 324) {
                    triangle_y1 = triangle_y1 - 21;
                    triangle_y2 = triangle_y2 - 21;
                    triangle_y3 = triangle_y3 - 21;
                }

                make_triangle(renderer, triangle_x1, triangle_y1,
                                        triangle_x2, triangle_y2,
                                        triangle_x3, triangle_y3,
                                        255, 255, 255, 255, 1);

            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN){
                if (battle_status == NORMAL) {
                    if (triangle_y3 <= 344) {
                        triangle_y1 = triangle_y1 + 21;
                        triangle_y2 = triangle_y2 + 21;
                        triangle_y3 = triangle_y3 + 21;
                    }
               } else if (battle_status == BATTLE_ACTION) {
                    if (triangle_y3 <= 365) {
                        triangle_y1 = triangle_y1 + 21;
                        triangle_y2 = triangle_y2 + 21;
                        triangle_y3 = triangle_y3 + 21;
                    }
              } else if (battle_status == BATTLE_SELECT) {
                    if (triangle_y3 <= battle + battle_enemy_window.rectangle_h - 21) {
                        triangle_y1 = triangle_y1 + 21;
                        triangle_y2 = triangle_y2 + 21;
                        triangle_y3 = triangle_y3 + 21;
                    }

                }

                make_triangle(renderer, triangle_x1, triangle_y1,
                                        triangle_x2, triangle_y2,
                                        triangle_x3, triangle_y3,
                                        255, 255, 255, 255, 1);
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                if (battle_status == NORMAL) {
                    if (triangle_y1 == escape) {
                        battle_status = ESCAPE;
                    } else if (triangle_y1 == battle) {
                        battle_status = BATTLE_ACTION;
                    }
           } else if (battle_status == BATTLE_ACTION) {
                    if (triangle_y1 == battle_select) {
                        battle_status = BATTLE_SELECT;
                    } else if (triangle_y1 == defense) {
                        battle_status = DEFENSE;
                    } else if (triangle_y1 == item) {
                        battle_status = ITEM;
                    }
          } else if (battle_status == BATTLE_SELECT) {
                    battle_status = ATTACK;
                    attack = triangle_y1;
                }
            }

        }
    }

    battle_enemy_window.rectangle_h = rectangle_h;
    sprintf(MAP_EVENT_NAME, "%s", map_event_name_save);
    free(monster_object);

    resume_sounds();
    load_bgm(MAP_EVENT_NAME);

    return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "RPGInC.h"
#include "effect/effect.h"
#include "sounds/sounds.h"

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
    int i, j;

    load_mapchip(renderer);
    load_map("data/field.map");

    load_image(renderer, &player_image, "image/charachip/chiharu.bmp");
    set_player_status();

    load_npc(renderer);
    load_monster(renderer);

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

        clac_offset(player.pixel_x, player.pixel_y, &player.offset_x, &player.offset_y);

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

int load_image(SDL_Renderer *renderer, SDL_Texture **image_texture, char *filename) {

    SDL_Surface *image = NULL;

    // 画像の読み込み
    image = IMG_Load(filename);
    if(!image) {
        printf("IMG_Load: %s\n", IMG_GetError());
        return 1;
    }

    // 透過色の設定
    SDL_SetColorKey(image, SDL_TRUE, SDL_MapRGB(image->format, 255, 0, 255));

    *image_texture = SDL_CreateTextureFromSurface(renderer, image);

    SDL_FreeSurface(image);

    return 0;
}

int player_animation(SDL_Renderer *renderer, SDL_Texture *player_image) {

    int x = ((frame / animecycle) % 4) * 16;
    int y = player.direction * IMAGE_HEIGHT;

    SDL_Rect imageRect=(SDL_Rect){x, y, IMAGE_WIDTH, IMAGE_HEIGHT};
    SDL_Rect drawRect=(SDL_Rect){player.pixel_x - player.offset_x, player.pixel_y - player.offset_y,
                                 IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

    SDL_RenderCopy(renderer, player_image, &imageRect, &drawRect);

    if (frame <= animecycle * 4) {
        frame += 1;
    } else{
        frame = 0;
    }

    return 0;
}

int player_update(SDL_Renderer *renderer, SDL_Event e, SDL_Texture *player_image) {

    if (player.moving == TRUE) {
        player.pixel_x = player.pixel_x + player.velocity_x;
        player.pixel_y = player.pixel_y + player.velocity_y;

        if (player.pixel_x % GRID_SIZE == 0 && player.pixel_y % GRID_SIZE == 0){
            player.moving = FALSE;
            player.map_x = player.pixel_x / GRID_SIZE;
            player.map_y = player.pixel_y / GRID_SIZE;


            load_move(renderer, player_image);

            if (number_of_monster != 0) {
                battle_encount(renderer, e);
            }

            player_move(e);

        }

    } else {
        player_move(e);
    }

}

int player_move(SDL_Event e) {

    if (message_window.visible == OUT_VISIBLE) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP){
            player.direction = UP;
            if (is_movable(player.map_x, player.map_y - 1) == TRUE) {
                player.velocity_x = 0;
                player.velocity_y = -speed;
                player.moving = TRUE;
            }
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN){
            player.direction = DOWN;
            if (is_movable(player.map_x, player.map_y + 1) == TRUE) {
                player.velocity_x = 0;
                player.velocity_y = speed;
                player.moving = TRUE;
            }
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT){
            player.direction = RIGHT;
            if (is_movable(player.map_x + 1, player.map_y) == TRUE) {
                player.velocity_x = speed;
                player.velocity_y = 0;
                player.moving = TRUE;
            }
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT){
            player.direction = LEFT;
            if (is_movable(player.map_x - 1, player.map_y) == TRUE) {
                player.velocity_x = -speed;
                player.velocity_y = 0;
                player.moving = TRUE;
            }
        }
    }

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

int load_monster(SDL_Renderer *renderer) {
    char event_path[256] = {0};

    sprintf(event_path, "data/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256] = {0};
    char monster_name[256] = {0};

    char buf[1024] = {0};
    char npc_path[256] = {0};
    int monster_id = 0;
    int i = 0;
    int element_number = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for (i = 0;i < 256;i++) {
        monster[i].monster_id = 0;
        strcpy(monster[i].status.name, "\0");
        monster[i].status.sex = 0;
        monster[i].status.level = 0;
        monster[i].status.hp = 0;
        monster[i].status.mp = 0;
        monster[i].status.strength = 0;
        monster[i].status.agility = 0;
        monster[i].status.protection = 0;
        monster[i].status.wisdom = 0;
        monster[i].status.luck = 0;
        strcpy(monster[i].skill1, "\0");
        strcpy(monster[i].skill2, "\0");
        strcpy(monster[i].skill3, "\0");
        strcpy(monster[i].skill4, "\0");
        strcpy(monster[i].skill5, "\0");
        strcpy(monster[i].item1, "\0");
        monster[i].item1_probability = 0;
        strcpy(monster[i].item2, "\0");
        monster[i].item2_probability = 0;
        strcpy(monster[i].item3, "\0");
        monster[i].item3_probability = 0;
        monster[i].gold = 0;
        monster[i].status.experience_point = 0;
    }

    for (i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "MONSTER", 7) == 0) {
                sscanf(buf, "%[^,],%[^,],%d", event, monster_name, &monster_id);

                load_monster_status(monster_name, monster_id, element_number);
                element_number++;
            }
        }

    }

    number_of_monster = element_number;

    fclose(fp);

    return 0;
}

int load_monster_status(char *in_monster_name, int in_monster_id, int element_number) {

    char *event_path =  "data/monster.dat";

    FILE *fp;
    char buf[1024] = {0};
    char event[256] = {0};

    int monster_id = 0;
    char monster_name[256] = {0};
    int sex = 0;
    int level = 0;
    int hp = 0;
    int mp = 0;
    int strength = 0;
    int agility = 0;
    int protection = 0;
    int wisdom = 0;
    int luck = 0;
    char skill1[256] = {0};
    char skill2[256] = {0};
    char skill3[256] = {0};
    char skill4[256] = {0};
    char skill5[256] = {0};
    char item1[256] = {0};
    double item1_probability = 0.0;
    char item2[256] = {0};
    double item2_probability = 0.0;
    char item3[256] = {0};
    double item3_probability = 0.0;
    int gold = 0;
    int experience_point = 0;
    double encounter_probability = 0.0;

    int i = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {
        if (strncmp(buf, "#", 1) != 0){
           sscanf(buf,
           "%d,%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%lf,%[^,],%lf,%[^,],%lf,%d,%d, %lf",
           &monster_id, monster_name,
           &sex, &level, &hp, &mp, &strength, &agility, &protection, &wisdom, &luck,
           skill1, skill2, skill3, skill4, skill5,
           item1, &item1_probability, item2, &item2_probability, item3, &item3_probability,
           &gold, &experience_point, &encounter_probability);

            if (strcmp(in_monster_name, monster_name) == 0 && in_monster_id ==  monster_id) {
                monster[element_number].monster_id = monster_id;
                strcpy(monster[element_number].status.name, monster_name);
                monster[element_number].status.sex = sex;
                monster[element_number].status.level = level;
                monster[element_number].status.hp = hp;
                monster[element_number].status.mp = mp;
                monster[element_number].status.strength = strength;
                monster[element_number].status.agility = agility;
                monster[element_number].status.protection = protection;
                monster[element_number].status.wisdom = wisdom;
                monster[element_number].status.luck = luck;
                strcpy(monster[element_number].skill1, skill1);
                strcpy(monster[element_number].skill2, skill2);
                strcpy(monster[element_number].skill3, skill3);
                strcpy(monster[element_number].skill4, skill4);
                strcpy(monster[element_number].skill5, skill5);
                strcpy(monster[element_number].item1, item1);
                monster[element_number].item1_probability = item1_probability;
                strcpy(monster[element_number].item2, item2);
                monster[element_number].item2_probability = item2_probability;
                strcpy(monster[element_number].item3, item3);
                monster[element_number].item3_probability = item3_probability;
                monster[element_number].gold = gold;
                monster[element_number].status.experience_point = experience_point;
                monster[element_number].encounter_probability = encounter_probability;
            }
        }
    }

    // printf("name:%s\n", monster[0].status.name);
    // printf("name:%s\n", monster[1].status.name);
    // printf("name:%s\n", monster[2].status.name);
    // printf("ddd:%d %s %d %d %d %d %d %d %d %d %d %s %s %s %s %s %s %lf %s %lf %s %lf %d %d %lf\n",
    // monster[element_number].monster_id, monster[element_number].status.name,
    // monster[element_number].status.sex, monster[element_number].status.level,
    // monster[element_number].status.hp, monster[element_number].status.mp,
    // monster[element_number].status.strength,
    // monster[element_number].status.agility,
    // monster[element_number].status.protection,
    // monster[element_number].status.wisdom,
    // monster[element_number].status.luck,
    // monster[element_number].skill1,
    // monster[element_number].skill2,
    // monster[element_number].skill3,
    // monster[element_number].skill4,
    // monster[element_number].skill5,
    // monster[element_number].item1,
    // monster[element_number].item1_probability,
    // monster[element_number].item2,
    // monster[element_number].item2_probability,
    // monster[element_number].item3,
    // monster[element_number].item3_probability,
    // monster[element_number].gold, monster[element_number].status.experience_point,
    // monster[element_number].encounter_probability
    // );

    return 0;
}

int load_npc(SDL_Renderer *renderer) {
    char event_path[256] = {0};

    sprintf(event_path, "data/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256] = {0};
    char npc_name[256] = {0};
    int map_x;
    int map_y;
    DIRECTION direction;
    MOVING moving;
    int max_step;
    int message_length;
    char message[1024] = {0};

    char buf[1024] = {0};
    char npc_path[256] = {0};
    int i = 0;
    int element_number = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for (i = 0;i < number_of_npc_image;i++) {
        npc[i].npc.map_x = 0;
        npc[i].npc.map_y = 0;
        npc[i].npc.pixel_x = 0;
        npc[i].npc.pixel_y = 0;
        npc[i].npc.direction = 0;
        npc[i].npc.moving = 0;
        SDL_DestroyTexture(npc[i].npc_image);
    }

    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "CHARA", 5) == 0) {
                sscanf(buf,
                   "%[^,],%[^,],%d,%d,%u,%u,%[^,]",
                       event, npc_name, &map_x, &map_y, &direction, &moving, message);

                sprintf(npc_path, "image/charachip/%s.bmp", npc_name);
                load_image(renderer, &npc[element_number].npc_image, npc_path);

                npc[element_number].npc.map_x = map_x;
                npc[element_number].npc.map_y = map_y;
                npc[element_number].npc.pixel_x = map_x * GRID_SIZE;
                npc[element_number].npc.pixel_y = map_y * GRID_SIZE;
                npc[element_number].npc.direction = direction;
                npc[element_number].npc_move = moving;

                message_length = strlen(message);
                message[message_length - 1] = '\0';
                sprintf(npc[element_number].message, "%s", message);

                element_number += 1;
            }
        }
    }

    number_of_npc_image = element_number;

    fclose(fp);

    return 0;
}

int npc_animation(SDL_Renderer *renderer) {

    int i;
    for(i = 0; i < number_of_npc_image;i++) {

        int x = ((frame / animecycle) % 4) * 16;
        int y = npc[i].npc.direction * IMAGE_HEIGHT;

        SDL_Rect imageRect=(SDL_Rect){x, y, IMAGE_WIDTH, IMAGE_HEIGHT};
        SDL_Rect drawRect=(SDL_Rect){npc[i].npc.pixel_x - player.offset_x,
                                     npc[i].npc.pixel_y - player.offset_y,
                                     IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

        SDL_RenderCopy(renderer, npc[i].npc_image, &imageRect, &drawRect);

        if (npc[i].npc_move == TRUE && number_of_npc_image != 0) {
            npc_update(renderer, i);
        }
    }

    return 0;
}

int npc_update(SDL_Renderer *renderer, int element) {

    srand((unsigned)time(NULL));
    int target = rand()%number_of_npc_image;
    int action = rand()%10;

    if (npc[element].npc_move == TRUE) {
        if (npc[element].npc.moving == TRUE) {
            npc[element].npc.pixel_x = npc[element].npc.pixel_x + npc[element].npc.velocity_x;
            npc[element].npc.pixel_y = npc[element].npc.pixel_y + npc[element].npc.velocity_y;

            if (npc[element].npc.pixel_x % GRID_SIZE == 0 && npc[element].npc.pixel_y % GRID_SIZE == 0) {
                npc[element].npc.moving = FALSE;
                npc[element].npc.map_x = npc[element].npc.pixel_x / GRID_SIZE;
                npc[element].npc.map_y = npc[element].npc.pixel_y / GRID_SIZE;
            }

        } else {
            if (target == element && action < 3) {
                npc_move(rand()%4, element);
            }
        }
    }
}

int npc_move(DIRECTION direction, int element) {

    if (message_window.visible == OUT_VISIBLE) {
        if (frame == 0) {
            if (direction == UP){
                npc[element].npc.direction = UP;
                if (is_movable(npc[element].npc.map_x, npc[element].npc.map_y - 1) == TRUE) {
                    npc[element].npc.velocity_x = 0;
                    npc[element].npc.velocity_y = -speed;
                    npc[element].npc.moving = TRUE;
                }
            } else if (direction == DOWN){
                npc[element].npc.direction = DOWN;
                if (is_movable(npc[element].npc.map_x, npc[element].npc.map_y + 1) == TRUE) {
                    npc[element].npc.velocity_x = 0;
                    npc[element].npc.velocity_y = speed;
                    npc[element].npc.moving = TRUE;
                }
            } else if (direction == RIGHT){
                npc[element].npc.direction = RIGHT;
                if (is_movable(npc[element].npc.map_x + 1, npc[element].npc.map_y) == TRUE) {
                    npc[element].npc.velocity_x = speed;
                    npc[element].npc.velocity_y = 0;
                    npc[element].npc.moving = TRUE;
                }
            } else if (direction == LEFT){
                npc[element].npc.direction = LEFT;
                if (is_movable(npc[element].npc.map_x - 1, npc[element].npc.map_y) == TRUE) {
                    npc[element].npc.velocity_x = -speed;
                    npc[element].npc.velocity_y = 0;
                    npc[element].npc.moving = TRUE;
                }
            }
        }
    }
    return 0;

}

int draw_map(SDL_Renderer *renderer){

    int x, y;
    int start_x = player.offset_x / GRID_SIZE - 1;
    int end_x = start_x + SCREEN_WIDTH / GRID_SIZE + 2;
    int start_y = player.offset_y / GRID_SIZE - 1;
    int end_y = start_y + SCREEN_HEIGHT/ GRID_SIZE + 2;

    for(y = start_y;y < end_y;y++){
        for(x = start_x; x < end_x;x++){
            SDL_Rect imageRect=(SDL_Rect){0, 0, IMAGE_WIDTH, IMAGE_HEIGHT};
            SDL_Rect drawRect=(SDL_Rect){(x * GRID_SIZE) - player.offset_x,
                                         (y * GRID_SIZE) - player.offset_y,
                                         IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

            if ((x < 0) || (x > COL - 1) || (y < 0) || (y > ROW - 1)){
                SDL_RenderCopy(renderer, mapchip[OUT_OF_MAP].map_image, &imageRect, &drawRect);
            } else {
                SDL_RenderCopy(renderer, mapchip[map_array[y*COL+x]].map_image, &imageRect, &drawRect);
            }

        }
    }

    return 0;
}

int draw_treasure(SDL_Renderer *renderer){

    int i, j;

    for(i = 0;i<sizeof(treasure)/sizeof(treasure[0]);i++){
        if (strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {
            for(j = 0;j<sizeof(treasure->treasure)/sizeof(treasure->treasure[0]);j++){
                if (strcmp(treasure[i].treasure[j].item, "empty") != 0) {
                    SDL_Rect imageRect=(SDL_Rect){0, 0, IMAGE_WIDTH, IMAGE_HEIGHT};
                    SDL_Rect drawRect=(SDL_Rect){(treasure[i].treasure[j].map_x * GRID_SIZE) - player.offset_x,
                                                 (treasure[i].treasure[j].map_y * GRID_SIZE) - player.offset_y,
                                                 IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

                    if (treasure[i].treasure[j].status == 0){
                        SDL_RenderCopy(renderer, mapchip[15].map_image, &imageRect, &drawRect);
                    } else {
                        SDL_RenderCopy(renderer, mapchip[16].map_image, &imageRect, &drawRect);
                    }
                }
            }
        }
    }

    return 0;
}

int draw_door(SDL_Renderer *renderer, SDL_Event e){

    int i;

    for(i = 0;i<sizeof(door)/sizeof(door[0]);i++){
        if (door[i].status != 9) {
            SDL_Rect imageRect=(SDL_Rect){0, 0, IMAGE_WIDTH, IMAGE_HEIGHT};
            SDL_Rect drawRect=(SDL_Rect){(door[i].map_x * GRID_SIZE) - player.offset_x,
                                         (door[i].map_y * GRID_SIZE) - player.offset_y,
                                         IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

            if (door[i].status == 0){
                SDL_RenderCopy(renderer, mapchip[17].map_image, &imageRect, &drawRect);
            }
        }
    }

    return 0;
}

int load_move(SDL_Renderer *renderer, SDL_Texture *player_image) {
    char event_path[256] = {0};

    sprintf(event_path, "data/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256] = {0};
    int event_point_x;
    int event_point_y;
    DIRECTION direction_of_penetration;
    char buf[256] = {0};
    char new_map_name[256] = {0};
    char map_path[256] = {0};
    int new_x;
    int new_y;
    int i = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "MOVE", 4) == 0) {
                sscanf(buf,
                   "%[^,],%d,%d,%u,%[^,],%d,%d",
                       event, &event_point_x, &event_point_y, &direction_of_penetration, new_map_name, &new_x, &new_y);

                if (player.map_x == event_point_x && player.map_y == event_point_y) {
                    if (player.direction == direction_of_penetration) {

                        load_se(player.map_x, player.map_y, MAP_EVENT_NAME);

                        sprintf(MAP_EVENT_NAME, "%s", new_map_name);

                        sprintf(map_path, "data/%s.map", new_map_name);
                        load_map(map_path);
                        load_treasure(renderer);
                        load_door(renderer);

                        load_bgm(MAP_EVENT_NAME);

                        player.map_x = new_x;
                        player.map_y = new_y;
                        player.pixel_x = player.map_x * GRID_SIZE;
                        player.pixel_y = player.map_y * GRID_SIZE;

                        load_npc(renderer);
                        load_monster(renderer);

                        fade_out(renderer, player_image);

                        break;
                    }
                }
            }
        }
    }

    fclose(fp);

    return 0;
}

int battle_encount(SDL_Renderer *renderer, SDL_Event e) {

    int target = rand()%number_of_monster;
    srand((unsigned)time(NULL));
    double encount = (double)rand()/RAND_MAX;

    // printf("encount:%lf\n", encount);
    // printf("monster:%lf\n", monster[target].encounter_probability);
    if (encount < monster[target].encounter_probability) {
        battle_window(renderer, e, monster[target]);
    } else {
        // printf("not encount\n");
    }

    return 0;
}

int draw_monster(SDL_Renderer *renderer, char *path, int num_of_monster, MONSTER *monster) {

    int i;
    int pos;
    int w, h;

    SDL_Texture *monster_image = NULL;
    load_image(renderer, &monster_image, path);

    SDL_QueryTexture(monster_image, NULL, NULL, &w, &h);

    SDL_Rect imageRect;
    SDL_Rect drawRect;

    if ((num_of_monster % 2) == 1) {
        pos = -1 * w/2 - (w * (num_of_monster / 2)) ;

        for (i=0;i<num_of_monster;i++) {
            if (0 < monster[i].status.hp) {
                imageRect=(SDL_Rect){0, 0, w, h};
                drawRect=(SDL_Rect){SCREEN_WIDTH/2+pos, SCREEN_HEIGHT/2-h/2, w, h};

                SDL_RenderCopy(renderer, monster_image, &imageRect, &drawRect);

                monster[i].monster_image = monster_image;
                monster[i].imageRect = imageRect;
                monster[i].drawRect = drawRect;
            }
            pos = pos + w;
        }
    } else if ((num_of_monster % 2) == 0) {
        pos = -1 * w * (num_of_monster/2);

        for (i=0;i<num_of_monster;i++) {
            if (0 < monster[i].status.hp) {
                imageRect=(SDL_Rect){0, 0, w, h};
                drawRect=(SDL_Rect){SCREEN_WIDTH/2+pos, SCREEN_HEIGHT/2-h/2, w, h};
                SDL_RenderCopy(renderer, monster_image, &imageRect, &drawRect);

                monster[i].monster_image = monster_image;
                monster[i].imageRect = imageRect;
                monster[i].drawRect = drawRect;
            }
            pos = pos + w;
        }
    }

    return 0;

}

int knock_out_monster(SDL_Renderer *renderer, SDL_Event e, char *path, int num_of_monster,
                      MONSTER *monster_object, char *mes_buf) {

    SDL_RenderClear(renderer);

    draw_map(renderer);
    draw_treasure(renderer);
    draw_door(renderer, e);
    npc_animation(renderer);

    make_window(renderer, battle_back_window);
    draw_monster(renderer, path, num_of_monster, monster_object);

    create_battle_status_window(renderer);

    state = ON;
    message_window_status();

    message = mes_buf;
    make_window(renderer, message_window);
    flush_message_engine(renderer, font, e);

    SDL_RenderPresent(renderer);

    return 0;
}

int create_battle_status_window(SDL_Renderer *renderer) {

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

    BATTLE_STATUS battle_status = NORMAL;
    SDL_Texture *blow_image = NULL;

    load_image(renderer, &blow_image, "image/effects/blow.bmp");

    sprintf(monster_name, "%s", monster.status.name);
    printf("monster name:%s\n", monster_name);
    sprintf(path, "image/monster/%s.bmp", monster.status.name);
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

        create_battle_status_window(renderer);

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

                    blow_effects(renderer, &blow_image,
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


                        knock_out_monster(renderer, e, path, num_of_monster, monster_object, mes_buf);
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

                        create_battle_status_window(renderer);
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
                blow_effects(renderer, &blow_image,
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

                    knock_out_monster(renderer, e, path, num_of_monster, monster_object, mes_buf);
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
                flash_triangle_status = ON;
                battle_status = BATTLE_END;
            }


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

                    create_battle_status_window(renderer);
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
    SDL_DestroyTexture(blow_image);

    resume_sounds();
    load_bgm(MAP_EVENT_NAME);

    return 0;
}

int compare_agility(const void * n1, const void * n2) {
    if (((MONSTER *)n1)->status.agility > ((MONSTER *)n2)->status.agility) {
        return -1;
    } else if (((MONSTER *)n1)->status.agility < ((MONSTER *)n2)->status.agility) {
        return 1;
    } else {
        return 0;
    }
}

int fade_out(SDL_Renderer *renderer, SDL_Texture *player_image) {

    SDL_Rect rectangle;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.w = SCREEN_WIDTH;
    rectangle.h = SCREEN_HEIGHT;

    int i = 200;
    int inverse_flg = 0;
    while(1) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, i);
        SDL_RenderFillRect(renderer, &rectangle);
        SDL_RenderPresent(renderer);

        if (inverse_flg == 0 && i <= 255) {
            i = i + 5;
            SDL_Delay(80);
        }

        if (i == 255) {
           inverse_flg = 1;
        }

        if (inverse_flg == 1) {
            clac_offset(player.pixel_x, player.pixel_y, &player.offset_x, &player.offset_y);
            draw_map(renderer);
            player_animation(renderer, player_image);
            SDL_Delay(20);

            i = i - 5;

            if (i == 200) {
                break;
            }
        }
    }

    return 0;

}



int load_treasure(SDL_Renderer *renderer) {
    char event_path[256] = {0};

    sprintf(event_path, "data/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256] = {0};
    int item_id;
    int map_x;
    int map_y;
    char item[128] = {0};
    int item_length;

    char buf[256] = {0};
    int i, j;
    int new_flg = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for (i = 0;i < sizeof(treasure)/sizeof(treasure[0]);i++) {
        if (strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {
            new_flg = 1;
        }
    }

    if (new_flg == 0) {
        i = 0;
        while (1) {
            if (strcmp(treasure[i].map, "empty") == 0) {
                sprintf(treasure[i].map, "%s", MAP_EVENT_NAME);
                break;
            }
            i++;
        }
    }

    for (i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "TREASURE", 8) == 0) {
                sscanf(buf,
                   "%[^,],%d,%d,%d,%[^,]",
                       event, &item_id, &map_x, &map_y, item);

                for (j = 0;j < sizeof(treasure)/sizeof(treasure[0]);j++) {
                    if (strcmp(treasure[j].map, MAP_EVENT_NAME) == 0) {
                        if (treasure[j].treasure[item_id].status != 1) {
                            treasure[j].treasure[item_id].map_x = map_x;
                            treasure[j].treasure[item_id].map_y = map_y;
                            treasure[j].treasure[item_id].status = 0;

                            item_length = strlen(item);
                            item[item_length - 1] = '\0';
                            sprintf(treasure[j].treasure[item_id].item, "%s", item);
                        }
                    }
                }

            }
        }
    }


    fclose(fp);

    return 0;
}

int load_door(SDL_Renderer *renderer) {
    char event_path[256] = {0};

    sprintf(event_path, "data/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256] = {0};
    int map_x;
    int map_y;

    char buf[256] = {0};
    int i;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for (i = 0;i < sizeof(door)/sizeof(door[0]);i++) {
        door[i].status = 9;
    }

    for (i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "DOOR", 4) == 0) {
                sscanf(buf,
                   "%[^,],%d,%d",
                       event, &map_x, &map_y);

                 door[i].map_x = map_x;
                 door[i].map_y = map_y;
                 door[i].status = 0;

            }
        }
    }


    fclose(fp);

    return 0;
}

int load_map(char *map_name) {
    FILE *fp;
    int i = 0;
    if ((fp = fopen(map_name, "rb")) == NULL) {
        return 1;
    }

    fread(&COL, sizeof(int), 1, fp);
    fread(&ROW, sizeof(int), 1, fp);
    fread(&OUT_OF_MAP, sizeof(int), 1, fp);

    map_array = realloc(map_array, sizeof(int) * COL * ROW);

    while(!feof(fp)) {
        fread(&map_array[i++], sizeof(int), 1, fp);
    }

    fclose(fp);

    return 0;
}

int load_mapchip(SDL_Renderer *renderer) {

    FILE *fp;
    int x, y, z;
    char n[256] = {0};
    char path[280] = {0};
    char buf[256] = {0};
    int i = 0;

    fp = fopen("data/mapchip.dat", "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++){
        if (strncmp(buf, "#", 1) != 0){
            sscanf(buf, "%d,%[^,],%d,%d", &x, n, &y, &z);
            mapchip[i].mapchip_id = x;
            strcpy(mapchip[i].mapchip_name, n);
            mapchip[i].movable = y;
            mapchip[i].change_locate = z;

            sprintf(path, "image/mapchip/%s.bmp", mapchip[i].mapchip_name);
            load_image(renderer, &mapchip[i].map_image, path);
        } else {
            i--;
        }

    }

    number_of_map_image = i - 1;

    fclose(fp);

    return 0;

}

int is_movable(int x, int y) {

    int i, j;
    for (i = 0;i < number_of_npc_image;i++) {
        if (npc[i].npc.map_x == x && npc[i].npc.map_y == y) {
            return TALK;
        }
    }

    for (i = 0;i < sizeof(treasure)/sizeof(treasure[0]);i++) {
        for (j = 0;j < sizeof(treasure->treasure)/sizeof(treasure->treasure[0]);j++) {
            if (treasure[i].treasure[j].map_x == x && treasure[i].treasure[j].map_y == y &&
                strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {
                return SEARCH;
            }
        }
    }

    for (i = 0;i < sizeof(door)/sizeof(door[0]);i++) {
        if (door[i].map_x == x && door[i].map_y == y && door[i].status == 0) {
            return OPEN;
        }
    }

    if ( x < 0 || x > COL - 1 || y  < 0 || y > ROW - 1) {
        sound_se("collisionOfWall.ogg");
        return 2;
    }

    if (mapchip[map_array[y*COL+x]].movable == 1) {
        sound_se("collisionOfWall.ogg");
        return 2;
    }

    if (player.map_x == x && player.map_y == y) {
        sound_se("collisionOfWall.ogg");
        return 1;
    }

    return TRUE;
}

int clac_offset(int x, int y, int *offset_x, int *offset_y) {
    *offset_x = x - (SCREEN_WIDTH / 2);
    *offset_y = y - (SCREEN_HEIGHT / 2);

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

int make_triangle(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int x3, int y3, int blend, int r, int g, int b, int present) {

    SDL_Point points[4] = {{x1, y1},{x2, y2}, {x3, y3}, {x1, y1}};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, blend);
    SDL_RenderDrawLines(renderer, points, 4);

    if (present == 0) {
        SDL_RenderPresent(renderer);
    }

    return 0;
}

int make_window(SDL_Renderer *renderer, WINDOW window) {

    int edge_size = 4;

    SDL_Rect rectangle;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    rectangle.x = window.rectangle_x;
    rectangle.y = window.rectangle_y;
    rectangle.w = window.rectangle_w;
    rectangle.h = window.rectangle_h;

    // WHITE
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, window.blend);
    SDL_RenderFillRect(renderer, &rectangle);

    rectangle.x = window.rectangle_x + edge_size;
    rectangle.y = window.rectangle_y + edge_size;
    rectangle.w = window.rectangle_w - edge_size * 2;
    rectangle.h = window.rectangle_h - edge_size * 2;

    // BLACK
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, window.blend);
    SDL_RenderFillRect(renderer, &rectangle);

    return 0;

}

int window_update(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e) {

    if (message_window.visible == IN_VISIBLE) {
        if (state == OFF) {
            state = ON;
            get_message();
        } else {
            make_window(renderer, message_window);
            message_engine(renderer, font, e);
            state = OFF;
        }
    }

    return 0;
}

int message_engine(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e) {

    int x;
    int message_length;
    int remaining_message_length;
    int word_length = 0;
    int byte_counter;
    int byt;

    int loop_counter = 0;
    int row_position = 0;
    int col_position = 0;
    int tmp_position = 0;

    const int row_size = 21;
    const int col_size = 84;
    const double start_x = 153.0;
    const double start_y = 354.0;
    const double inter_char = 5.35;

    char message_tmp[1024];
    char message_out[100];
    char word[6];
    char isspace[3] = {0};
    char isasterisk[3] = {0};

    char *se_file = "conversation.ogg";

    sound_se(se_file);

    strcpy(message_tmp, message);
    message_length = strlen(message);

    while (*message != '\0') {
        SDL_Delay(60);

        byt = u8mb(*message);
        message += u8mb(*message);

        remaining_message_length = strlen(message);

        memset(word, '\0', 6);

        byte_counter = 0;
        for (x=word_length;x<message_length - remaining_message_length;x++) {
            sprintf(&word[byte_counter], "%c", message_tmp[x]);
            byte_counter++;
        }

        // 改行判定(半角スペースが2つ続いたら改行とみなす)
        if (*word == ' ') {
            if (strcmp(isspace, "  ") == 0) {
                memset(isspace, '\0', 3);
            }
            strncat(isspace, word, 1);
        } else {
            memset(isspace, '\0', 3);
        }

        if (strcmp(isspace, "  ") == 0) {
            if (loop_counter % col_size != 0) {
                loop_counter = loop_counter + (row_size - (loop_counter % row_size) - 1);
            }
        }

        // 改ページ判定(アスタリスクが2つ続いたら改ページとみなす)
        if (*word == '*') {
            if (strcmp(isasterisk, "**") == 0) {
                memset(isasterisk, '\0', 3);
            }
            strncat(isasterisk, word, 1);
            strncpy(word, " ", 1);
        } else {
            memset(isasterisk, '\0', 3);
        }

        if (strcmp(isasterisk, "**") == 0) {
            loop_counter = loop_counter + (col_size - (loop_counter % col_size) - 1);
        }

        // 折り返し判定
        if (loop_counter != 0 && loop_counter % row_size == 0) {
            row_position += row_size;
            col_position = 0;
            tmp_position = word_length;
            if (loop_counter % col_size == 0 ) {
                while (1) {
                    flash_triangle(renderer);

                    if ( SDL_PollEvent(&e) ) {
                        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                            sound_se(se_file);
                            break;
                        }
                    }
                }
                make_window(renderer, message_window);
                row_position = 0;
                col_position = 0;
            } else {
                sound_se(se_file);
            }
        } else if (loop_counter % row_size > 0) {
            col_position = word_length - tmp_position;
        }

        display_character_string(renderer, font, word, start_x + col_position * inter_char, start_y + row_position, 0);

        word_length = message_length - remaining_message_length;

        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                col_position = col_position + byt;
                loop_counter++;

                memset(message_out, '\0', 100);

                while (*message != '\0') {
                    byt = u8mb(*message);
                    message += u8mb(*message);

                    remaining_message_length = strlen(message);

                    memset(word, '\0', 6);

                    byte_counter = 0;
                    for (x=word_length;x<message_length - remaining_message_length;x++) {
                        sprintf(&word[byte_counter], "%c", message_tmp[x]);
                        byte_counter++;
                    }

                    // 改行判定(半角スペースが2つ続いたら改行とみなす)
                    if (*word == ' ') {
                        if (strcmp(isspace, "  ") == 0) {
                            memset(isspace, '\0', 3);
                        }
                        strncat(isspace, word, 1);
                    } else {
                        memset(isspace, '\0', 3);
                    }

                    if (strcmp(isspace, "  ") == 0) {
                        if (loop_counter % col_size != 0) {
                            loop_counter = loop_counter + (row_size - (loop_counter % row_size) - 1);
                        }
                    }

                    // 改ページ判定(アスタリスクが2つ続いたら改ページとみなす)
                    if (*word == '*') {
                        if (strcmp(isasterisk, "**") == 0) {
                            memset(isasterisk, '\0', 3);
                        }
                        strncat(isasterisk, word, 1);
                        strncpy(word, " ", 1);
                    } else {
                        memset(isasterisk, '\0', 3);
                    }

                    if (strcmp(isasterisk, "**") == 0) {
                        loop_counter = loop_counter + (col_size - (loop_counter % col_size) - 1);
                    }

                    // 折り返し判定
                    if (loop_counter % row_size == 0) {
                        display_character_string(renderer, font, message_out, start_x + col_position * inter_char, start_y + row_position, 0);
                        row_position += row_size;
                        col_position = 0;
                        tmp_position = word_length;
                        memset(message_out, '\0', 100);

                        if (loop_counter % col_size == 0 ) {
                            while (1) {
                                flash_triangle(renderer);

                                if ( SDL_PollEvent(&e) ) {
                                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                                        break;
                                    }
                                }
                            }
                            make_window(renderer, message_window);
                            row_position = 0;
                            col_position = 0;
                        }
                    }

                    strcat(message_out, word);
                    word_length = message_length - remaining_message_length;

                    loop_counter++;
                }
                display_character_string(renderer, font, message_out, start_x + col_position * inter_char, start_y + row_position, 0);

                break;
            }
        }

        loop_counter++;
    }

    if (flash_triangle_status == ON) {
        if (message_window.visible == IN_VISIBLE) {
            while (1) {
                flash_triangle(renderer);

                if ( SDL_PollEvent(&e) ) {
                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                        message_window.visible = OUT_VISIBLE;
                        break;
                    }
                }
            }
        }
    } else {
        SDL_Delay(300);
        message_window.visible = OUT_VISIBLE;
    }

    return 0;
}

int flush_message_engine(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e) {

    int x;
    int message_length;
    int remaining_message_length;
    int word_length = 0;
    int byte_counter;
    int byt;

    int loop_counter = 0;
    int row_position = 0;
    int col_position = 0;
    int tmp_position = 0;

    const int row_size = 21;
    const int col_size = 84;
    const double start_x = 153.0;
    const double start_y = 354.0;
    const double inter_char = 5.35;

    char message_tmp[1024] = {0};
    char message_out[100] = {0};
    char word[6] = {0};
    char isspace[3] = {0};
    char isasterisk[3] = {0};


    strcpy(message_tmp, message);
    message_length = strlen(message);

    while (*message != '\0') {
        byt = u8mb(*message);
        message += u8mb(*message);

        remaining_message_length = strlen(message);

        memset(word, '\0', 6);

        byte_counter = 0;
        for (x=word_length;x<message_length - remaining_message_length;x++) {
            sprintf(&word[byte_counter], "%c", message_tmp[x]);
            byte_counter++;
        }

        // 改行判定(半角スペースが2つ続いたら改行とみなす)
        if (*word == ' ') {
            if (strcmp(isspace, "  ") == 0) {
                memset(isspace, '\0', 3);
            }
            strncat(isspace, word, 1);
        } else {
            memset(isspace, '\0', 3);
        }

        if (strcmp(isspace, "  ") == 0) {
            if (loop_counter % col_size != 0) {
                loop_counter = loop_counter + (row_size - (loop_counter % row_size) - 1);
            }
        }

        // 改ページ判定(アスタリスクが2つ続いたら改ページとみなす)
        if (*word == '*') {
            if (strcmp(isasterisk, "**") == 0) {
                memset(isasterisk, '\0', 3);
            }
            strncat(isasterisk, word, 1);
            strncpy(word, " ", 1);
        } else {
            memset(isasterisk, '\0', 3);
        }

        if (strcmp(isasterisk, "**") == 0) {
            loop_counter = loop_counter + (col_size - (loop_counter % col_size) - 1);
        }

        // 折り返し判定
        if (loop_counter != 0 &&loop_counter % row_size == 0) {
            display_character_string(renderer, font, message_out, start_x + col_position * inter_char, start_y + row_position, 0);
            row_position += row_size;
            col_position = 0;
            tmp_position = word_length;
            memset(message_out, '\0', 100);
        }

        strcat(message_out, word);
        word_length = message_length - remaining_message_length;

        loop_counter++;
    }
    display_character_string(renderer, font, message_out, start_x + col_position * inter_char, start_y + row_position, 0);

    return 0;
}

int get_treasure_message() {

    int i, j;
    char *se_file = "treasure.ogg";

    for (i = 0;i < sizeof(treasure)/sizeof(treasure[0]);i++) {
        for (j = 0;j < sizeof(treasure->treasure)/sizeof(treasure->treasure[0]);j++) {
            if (player.direction == UP) {
                if (treasure[i].treasure[j].map_x == player.map_x &&
                    treasure[i].treasure[j].map_y == player.map_y - 1 &&
                    strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {

                    sound_se(se_file);

                    if (treasure[i].treasure[j].status == 0) {
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 24);
                        treasure[i].treasure[j].status = 1;
                    } else {
                        message = TRESUREBOX_EMPTY_MESSAGE;
                    }

                    return 0;
                }
            } else if (player.direction == DOWN) {
                if (treasure[i].treasure[j].map_x == player.map_x &&
                    treasure[i].treasure[j].map_y == player.map_y + 1 &&
                    strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {

                    sound_se(se_file);

                    if (treasure[i].treasure[j].status == 0) {
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 24);
                        treasure[i].treasure[j].status = 1;
                    } else {
                        message = TRESUREBOX_EMPTY_MESSAGE;
                    }

                    return 0;

                }
           } else if (player.direction == RIGHT) {
                if (treasure[i].treasure[j].map_x == player.map_x + 1 &&
                    treasure[i].treasure[j].map_y == player.map_y &&
                    strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {

                    sound_se(se_file);

                    if (treasure[i].treasure[j].status == 0) {
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 24);
                        treasure[i].treasure[j].status = 1;
                    } else {
                        message = TRESUREBOX_EMPTY_MESSAGE;
                    }

                    return 0;

                }
           } else if (player.direction == LEFT) {
                if (treasure[i].treasure[j].map_x == player.map_x - 1 &&
                    treasure[i].treasure[j].map_y == player.map_y &&
                    strcmp(treasure[i].map, MAP_EVENT_NAME) == 0) {

                    sound_se(se_file);

                    if (treasure[i].treasure[j].status == 0) {
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 24);
                        treasure[i].treasure[j].status = 1;
                    } else {
                        message = TRESUREBOX_EMPTY_MESSAGE;
                    }

                    return 0;

                }
            }
        }
    }

    message = SEARCH_MESSAGE;

    return 0;
}

int get_npc_message() {

    int i;

    for(i = 0;i < number_of_npc_image;i++) {
        if (player.direction == UP) {
            if (npc[i].npc.map_x == player.map_x && npc[i].npc.map_y == player.map_y - 1) {
                npc[i].npc.direction = DOWN;
                message = npc[i].message;
                return 0;
            }
        } else if (player.direction == DOWN) {
            if (npc[i].npc.map_x == player.map_x && npc[i].npc.map_y == player.map_y + 1) {
                npc[i].npc.direction = UP;
                message = npc[i].message;
                return 0;
            }
        } else if (player.direction == RIGHT) {
            if (npc[i].npc.map_x == player.map_x + 1 && npc[i].npc.map_y == player.map_y) {
                npc[i].npc.direction = LEFT;
                message = npc[i].message;
                return 0;
            }
        } else if (player.direction == LEFT) {
            if (npc[i].npc.map_x == player.map_x - 1 && npc[i].npc.map_y == player.map_y) {
                npc[i].npc.direction = RIGHT;
                message = npc[i].message;
                return 0;
            }
        }
    }

    message = TALK_MESSAGE;

    return 0;
}

int get_message() {

    int i;

    message = SEARCH_MESSAGE;

    if (player.direction == UP) {
        if (is_movable(player.map_x, player.map_y - 1) == TALK) {
            get_npc_message();
        } else if (is_movable(player.map_x, player.map_y - 1) == SEARCH) {
            get_treasure_message();
        } else if (is_movable(player.map_x, player.map_y - 1) == OPEN) {
            open_door();
            message_window_status();
            state = OFF;
        }
    } else if (player.direction == DOWN) {
        if (is_movable(player.map_x, player.map_y + 1) == TALK) {
            get_npc_message();
        } else if (is_movable(player.map_x, player.map_y + 1) == SEARCH) {
            get_treasure_message();
        } else if (is_movable(player.map_x, player.map_y + 1) == OPEN) {
            open_door();
            message_window_status();
            state = OFF;
        }
    } else if (player.direction == RIGHT) {
        if (is_movable(player.map_x + 1, player.map_y) == TALK) {
            get_npc_message();
        } else if (is_movable(player.map_x + 1, player.map_y) == SEARCH) {
            get_treasure_message();
        } else if (is_movable(player.map_x + 1, player.map_y) == OPEN) {
            open_door();
            message_window_status();
            state = OFF;
        }
    } else if (player.direction == LEFT) {
        if (is_movable(player.map_x - 1, player.map_y) == TALK) {
            get_npc_message();
        } else if (is_movable(player.map_x - 1, player.map_y) == SEARCH) {
            get_treasure_message();
        } else if (is_movable(player.map_x - 1, player.map_y) == OPEN) {
            open_door();
            message_window_status();
            state = OFF;
        }
    }

    return 0;

}

int display_character_string(SDL_Renderer *renderer, TTF_Font *font, char *string, double x, double y, int present) {
    SDL_Surface *surface;
    SDL_Texture *texture;


    surface = TTF_RenderUTF8_Blended(font, string, (SDL_Color){255,255,255,255});

    texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    int iw,ih;
    SDL_QueryTexture(texture, NULL, NULL, &iw, &ih);

    SDL_Rect txtRect=(SDL_Rect){0,0,iw,ih};
    SDL_Rect pasteRect=(SDL_Rect){x,y,iw,ih};

    SDL_RenderCopy(renderer, texture, &txtRect, &pasteRect);

    if (present == 0) {
        SDL_RenderPresent(renderer);
    }

    SDL_FreeSurface(surface);

    return 0;
}

// UTF-8文字の1バイト目を判定して文字のバイト数を返す関数
int u8mb(const char chr)
{
    int byt = 1;
    if ((chr & 0x80) == 0x00) { //1byte文字は何もしない（[byt = 1]のまま）
    } else if ((chr & 0xE0) == 0xC0) { //2byte文字
        byt = 2;
    } else if ((chr & 0xF0) == 0xE0) { //3byte文字
        byt = 3;
    } else if ((chr & 0xF8) == 0xF0) { //4byte文字
        byt = 4;
    } else if ((chr & 0xFC) == 0xF8) { //5byte文字
        byt = 5;
    } else if ((chr & 0xFE) == 0xFC) { //6byte文字
        byt = 6;
    }
    return byt;
}

int flash_triangle(SDL_Renderer *renderer) {
    SDL_Delay(400);

    if (state == ON) {
        make_triangle(renderer, 310, 450, 320, 450, 315, 460, 255, 255, 255, 255, 0);
        state = OFF;
    } else {
        make_triangle(renderer, 310, 450, 320, 450, 315, 460, 255, 0, 0, 0, 0);
        state = ON;
    }

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

int open_door() {
    int i;
    char *se_file = "door.ogg";

    for(i = 0;i<sizeof(door)/sizeof(door[0]);i++){
        if (player.direction == UP) {
            if (door[i].map_x == player.map_x && door[i].map_y == player.map_y - 1) {
                door[i].status = 1;
                sound_se(se_file);
                return 0;
            }
        } else if (player.direction == DOWN) {
            if (door[i].map_x == player.map_x && door[i].map_y == player.map_y + 1) {
                door[i].status = 1;
                sound_se(se_file);
                return 0;
            }
        } else if (player.direction == RIGHT) {
            if (door[i].map_x == player.map_x + 1 && door[i].map_y == player.map_y) {
                door[i].status = 1;
                sound_se(se_file);
                return 0;
            }
        } else if (player.direction == LEFT) {
            if (door[i].map_x == player.map_x - 1 && door[i].map_y == player.map_y) {
                door[i].status = 1;
                sound_se(se_file);
                return 0;
            }
        }
    }

    message_window_status();
    message = DOOR_MESSAGE;

    return 0;
}

int message_window_status() {
    if (message_window.visible == OUT_VISIBLE) {
        message_window.visible = IN_VISIBLE;
    } else {
        message_window.visible = OUT_VISIBLE;
    }

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

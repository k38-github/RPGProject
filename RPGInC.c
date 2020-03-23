#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "RPGInC.h"

int main (int argc, char *argv[]) {

    // Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    window = SDL_CreateWindow( "DRAW IMAGE TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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

    // Initialize Mixer
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf( "Mixer could not initialize! Mixer_Error: %s\n", Mix_GetError());
        return 1;
    }
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
        printf( "Mix_OpenAudio could not initialize! Mixer_Error: %s\n", Mix_GetError());
        return 1;
    }
    //

    // Initialize Data
    int i, j;

    load_mapchip(renderer);
    load_map("data/field.map");

    SDL_Texture *player_image = NULL;
    load_image(renderer, &player_image, "image/charachip/chiharu.bmp");
    set_player_status();

    load_npc(renderer);

    for (i = 0;i < sizeof(treasure)/sizeof(treasure[0]);i++) {
        strcpy(treasure[i].map, "empty");

        for (j = 0;j < sizeof(treasure->treasure)/sizeof(treasure->treasure[0]);j++) {
            strcpy(treasure[i].treasure[j].item, "empty");
        }
    }

    for (i = 0;i < sizeof(door)/sizeof(door[0]);i++) {
        door[i].status = 9;
    }

    music = Mix_LoadMUS("music/bgm/filed.ogg");
    if (music == NULL) {
        return 1;
    }
    if (Mix_PlayMusic(music, -1) == -1) {
        return 1;
    }

    Mix_VolumeMusic(10);
    Mix_PlayingMusic();
    //

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

        if (back_flg > 0) {
	    if (back_flg == 1) {
                back_flg = 0;
            }
            make_commands_window(renderer, font, e);
        }

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

    // quit SDL_mixer
    Mix_FreeMusic(music);
    Mix_CloseAudio();

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
    player.status.name = "ちはる";
    player.status.level = 1;
    player.status.hp = 20;
    player.status.mp = 5;
    player.status.max_hp = 20;
    player.status.max_mp = 20;

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

                        load_se();

                        sprintf(MAP_EVENT_NAME, "%s", new_map_name);

                        sprintf(map_path, "data/%s.map", new_map_name);
                        load_map(map_path);
                        load_treasure(renderer);
                        load_door(renderer);

                        load_bgm();

                        player.map_x = new_x;
                        player.map_y = new_y;
                        player.pixel_x = player.map_x * GRID_SIZE;
                        player.pixel_y = player.map_y * GRID_SIZE;

                        load_npc(renderer);

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

int load_se(void) {
    char event_path[256] = {0};

    sprintf(event_path, "data/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char buf[256] = {0};
    char event[256] = {0};
    int event_point_x;
    int event_point_y;
    char *se_name;
    char se_path[256] = {0};

    int i = 0;

    Mix_Chunk *wave = NULL;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    if ((se_name = malloc(sizeof(char) * 256)) == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "SE", 2) == 0) {
                sscanf(buf,
                   "%[^,],%d,%d,%[^,]",
                       event, &event_point_x, &event_point_y, se_name);

                if (player.map_x == event_point_x && player.map_y == event_point_y) {
                    sprintf(se_path, "music/se/%s", se_name);

                    wave = Mix_LoadWAV(se_path);
                    if (wave == NULL) {
                        return 1;
                    }

                    Mix_VolumeChunk(wave, 10);
                    if ( Mix_PlayChannel(-1, wave, 0) == -1 ) {
                        return 1;
                    }

                    break;
                }
            }
        }
    }

    fclose(fp);
    free(se_name);

    return 0;
}

int sound_se(char *se_name) {

    Mix_Chunk *wave = NULL;
    char se_path[256] = {0};

    sprintf(se_path, "music/se/%s", se_name);

    wave = Mix_LoadWAV(se_path);
    if (wave == NULL) {
        return 1;
    }

    Mix_VolumeChunk(wave, 10);
    if ( Mix_PlayChannel(-1, wave, 0) == -1 ) {
        return 1;
    }

    return 0;
}

int load_bgm(void) {
    char event_path[256] = {0};

    sprintf(event_path, "data/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char buf[256] = {0};
    char event[256] = {0};
    char *bgm_name;
    char bgm_path[256] = {0};

    int i = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    if ((bgm_name = malloc(sizeof(char) * 256)) == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }


    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "BGM", 3) == 0) {
                sscanf(buf,
                   "%[^,],%[^,]",
                       event, bgm_name);

                sprintf(bgm_path, "music/bgm/%s", bgm_name);

                music = Mix_LoadMUS(bgm_path);
                if (music == NULL) {
                    return 1;
                }
                if (Mix_PlayMusic(music, -1) == -1) {
                    return 1;
                }

                break;
            }
        }
    }

    fclose(fp);
    free(bgm_name);

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
        sscanf(buf, "%d,%[^,],%d,%d", &x, n, &y, &z);
        mapchip[i].mapchip_id = x;
        strcpy(mapchip[i].mapchip_name, n);
        mapchip[i].movable = y;
        mapchip[i].change_locate = z;

        sprintf(path, "image/mapchip/%s.bmp", mapchip[i].mapchip_name);
        load_image(renderer, &mapchip[i].map_image, path);

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
        return 2;
    }

    if (mapchip[map_array[y*COL+x]].movable == 1) {
        return 2;
    }

    if (player.map_x == x && player.map_y == y) {
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
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 21);
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
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 21);
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
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 21);
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
                        message = strncat(treasure[i].treasure[j].item, TRESUREBOX_MESSAGE, 21);
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

    while (1) {

        check_command_status(&command_status, triangle_x1, triangle_y1);
        SDL_RenderPresent(renderer);

        if (back_flg == 2) {
            back_flg = 1;
            make_status_window(renderer, font, e);
	    break;
        }

        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                if (command_status == TALK) {
                    state = ON;
                    message_window_status();
                    get_npc_message();
                    break;
                } else if (command_status == MEMORY) {
                } else if (command_status == STATUS) {
		    back_flg = 1;
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

    get_status_triangle(&triangle_x1, &triangle_y1,
                        &triangle_x2, &triangle_y2,
                        &triangle_x3, &triangle_y3);


    make_window(renderer, status_window);
    make_triangle(renderer, triangle_x1, triangle_y1,
                            triangle_x2, triangle_y2,
                            triangle_x3, triangle_y3,
                            255, 255, 255, 255, 1);


    display_character_string(renderer, font, "つよさ",   70.0, 50.0,  1);
    //display_character_string(renderer, font, "-------------", 70.0, 65.0,  1);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 70, 73, 180, 73);
    display_character_string(renderer, font, "HPとMP",   70.0, 80.0,  1);
    display_character_string(renderer, font, "こうげきりょく",   70.0, 105.0, 1);
    display_character_string(renderer, font, "つよさをみる",   70.0, 130.0, 1);

    while (1) {

        check_status_status(&status_status, triangle_x1, triangle_y1);
        SDL_RenderPresent(renderer);

        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                if (status_status == HP_AND_MP) {
                    back_flg = 2;
                    make_hp_and_mp_window(renderer, font, e);
                    break;
		}
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP){
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


    display_character_string(renderer, font, "HPとMP",             70.0, 60.0,  1);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 70, 83, 200, 83);

    display_character_string(renderer, font, "レベル：",           70.0, 90.0,  1);
    convert_int_to_full_width_char(player.status.level, level);
    display_character_string(renderer, font, level,                135.0, 90.0,  1);

    display_character_string(renderer, font, "なまえ：",           70.0, 115.0, 1);
    display_character_string(renderer, font, player.status.name,   135.0, 115.0, 1);

    display_character_string(renderer, font, "H",                  70.0, 140.0, 1);
    convert_int_to_full_width_char(player.status.hp, hp);
    display_character_string(renderer, font, hp,                   110.0, 140.0, 1);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 90, 163, 170, 163);
    convert_int_to_full_width_char(player.status.max_hp, max_hp);
    display_character_string(renderer, font, max_hp,               110.0, 170.0, 1);

    display_character_string(renderer, font, "M",                  70.0, 195.0, 1);
    convert_int_to_full_width_char(player.status.mp, mp);
    display_character_string(renderer, font, mp,                   110.0, 195.0, 1);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 90, 218, 170, 218);
    convert_int_to_full_width_char(player.status.max_mp, max_mp);
    display_character_string(renderer, font, max_mp,               110.0, 225.0, 1);

    while (1) {

        SDL_RenderPresent(renderer);

        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
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

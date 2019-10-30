#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "RPGInC.h"

#define FONT_PATH "font/PixelMplus12-Regular.ttf"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int IMAGE_WIDTH = 16;
const int IMAGE_HEIGHT = 16;
const int MAGNIFICATION = 2;
const int GRID_SIZE = 32;
const int FONT_SIZE = 16;
int ROW = 15;
int COL = 20;
int OUT_OF_MAP = 0;
char MAP_EVENT_NAME[256] = "field";

int animecycle = 24;
int speed = 2;
int frame = 0;
int number_of_map_image = 0;
int number_of_npc_image = 0;

CARACTER player = {1, 1, 32, 32, 0, 0, 0, 0, DOWN, FALSE};
NPC npc[256] = {0};

MAPCHIP mapchip[256] = {0};

int *map_array;

WINDOW message_window = {140, 334, 360, 140, 255, OUT_VISIBLE};

char *message;
STATE state = OFF;

Mix_Music *music = NULL;

int main (int argc, char *argv[]) {

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = NULL;

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

    load_mapchip(renderer);
    load_map("data/field.map");

    SDL_Texture *player_image = NULL;
    load_image(renderer, &player_image, "image/charachip/chiharu.bmp");

    load_npc(renderer);
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

        npc_animation(renderer);

        player_animation(renderer, player_image);
        player_update(renderer, e, player_image);

        window_update(renderer, font, e);

        SDL_RenderPresent(renderer);

        // event handling
        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT){
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
                break;
            }
        }

    }

    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    int i;
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
            if (is_movable(player.map_x, player.map_y - 1) == 0) {
                player.velocity_x = 0;
                player.velocity_y = -speed;
                player.moving = TRUE;
            }
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN){
            player.direction = DOWN;
            if (is_movable(player.map_x, player.map_y + 1) == 0) {
                player.velocity_x = 0;
                player.velocity_y = speed;
                player.moving = TRUE;
            }
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT){
            player.direction = RIGHT;
            if (is_movable(player.map_x + 1, player.map_y) == 0) {
                player.velocity_x = speed;
                player.velocity_y = 0;
                player.moving = TRUE;
            }
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT){
            player.direction = LEFT;
            if (is_movable(player.map_x - 1, player.map_y) == 0) {
                player.velocity_x = -speed;
                player.velocity_y = 0;
                player.moving = TRUE;
            }
        }
    }

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
    for(i = 0; number_of_npc_image >= i;i++) {

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
                if (is_movable(npc[element].npc.map_x, npc[element].npc.map_y - 1) == 0) {
                    npc[element].npc.velocity_x = 0;
                    npc[element].npc.velocity_y = -speed;
                    npc[element].npc.moving = TRUE;
                }
            } else if (direction == DOWN){
                npc[element].npc.direction = DOWN;
                if (is_movable(npc[element].npc.map_x, npc[element].npc.map_y + 1) == 0) {
                    npc[element].npc.velocity_x = 0;
                    npc[element].npc.velocity_y = speed;
                    npc[element].npc.moving = TRUE;
                }
            } else if (direction == RIGHT){
                npc[element].npc.direction = RIGHT;
                if (is_movable(npc[element].npc.map_x + 1, npc[element].npc.map_y) == 0) {
                    npc[element].npc.velocity_x = speed;
                    npc[element].npc.velocity_y = 0;
                    npc[element].npc.moving = TRUE;
                }
            } else if (direction == LEFT){
                npc[element].npc.direction = LEFT;
                if (is_movable(npc[element].npc.map_x - 1, npc[element].npc.map_y) == 0) {
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

    int i;
    for(i = 0;i < number_of_npc_image;i++) {
        if (npc[i].npc.map_x == x && npc[i].npc.map_y == y) {
            return 1;
        }
    }

    if ( x < 0 || x > COL - 1 || y  < 0 || y > ROW - 1) {
        return 2;
    }

    if (mapchip[map_array[y*COL+x]].movable == 1) {
        return 2;
    }

    if(player.map_x == x && player.map_y == y) {
        return 1;
    }

    return 0;
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
    SDL_RenderPresent(renderer);

    return 0;
}

int make_triangle(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int x3, int y3, int blend, int r, int g, int b) {

    SDL_Point points[4] = {{x1, y1},{x2, y2}, {x3, y3}, {x1, y1}};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, blend);
    SDL_RenderDrawLines(renderer, points, 4);
    SDL_RenderPresent(renderer);

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

int window_engine(SDL_Renderer *renderer, WINDOW window) {

    make_window(renderer, window);

    return 0;
}

int window_update(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e) {


    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
        if (message_window.visible == OUT_VISIBLE) {
            message_window.visible = IN_VISIBLE;
        } else {
            message_window.visible = OUT_VISIBLE;
        }
    }

    if (message_window.visible == IN_VISIBLE) {
        window_engine(renderer, message_window);
        message_engine(renderer, font, e);
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

    if (state == OFF) {
        get_character_message(e, &message);
        state = ON;
    } else {
        sound_se("conversation.ogg");

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
                                sound_se("conversation.ogg");
                                break;
                            }
                        }
                    }
                    make_window(renderer, message_window);
                    row_position = 0;
                    col_position = 0;
                } else {
                    sound_se("conversation.ogg");
                }
            } else if (loop_counter % row_size > 0) {
                col_position = word_length - tmp_position;
            }

            display_character_string(renderer, font, word, start_x + col_position * inter_char, start_y + row_position);

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
                            display_character_string(renderer, font, message_out, start_x + col_position * inter_char, start_y + row_position);
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
                    display_character_string(renderer, font, message_out, start_x + col_position * inter_char, start_y + row_position);

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

        state = OFF;
    }

    return 0;
}

int get_character_message(SDL_Event e, char **message) {

    int i;
    *message = "そっちには　だれも　いないよ！";

    if (player.direction == UP) {
        if (is_movable(player.map_x, player.map_y - 1) == 1) {
            for(i = 0;i < number_of_npc_image;i++) {
                if (npc[i].npc.map_x == player.map_x && npc[i].npc.map_y == player.map_y - 1) {
                    npc[i].npc.direction = DOWN;
                    *message = npc[i].message;
                    break;
                }
            }
        }    } else if (player.direction == DOWN) {
        if (is_movable(player.map_x, player.map_y + 1) == 1) {
            for(i = 0;i < number_of_npc_image;i++) {
                if (npc[i].npc.map_x == player.map_x && npc[i].npc.map_y == player.map_y + 1) {
                    npc[i].npc.direction = UP;
                    *message = npc[i].message;
                    break;
                }
            }
        }    } else if (player.direction == RIGHT) {
        if (is_movable(player.map_x + 1, player.map_y) == 1) {
            for(i = 0;i < number_of_npc_image;i++) {
                if (npc[i].npc.map_x == player.map_x + 1 && npc[i].npc.map_y == player.map_y) {
                    npc[i].npc.direction = LEFT;
                    *message = npc[i].message;
                    break;
                }
            }
        }     } else if (player.direction == LEFT) {
        if (is_movable(player.map_x - 1, player.map_y) == 1) {
            for(i = 0;i < number_of_npc_image;i++) {
                if (npc[i].npc.map_x == player.map_x - 1 && npc[i].npc.map_y == player.map_y) {
                    npc[i].npc.direction = RIGHT;
                    *message = npc[i].message;
                    break;
                }
            }
        }
    }

    return 0;

}

int display_character_string(SDL_Renderer *renderer, TTF_Font *font, char *string, double x, double y) {
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
    SDL_RenderPresent(renderer);

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
        make_triangle(renderer, 310, 450, 320, 450, 315, 460, 255, 255, 255, 255);
        state = OFF;
    } else {
        make_box(renderer, 310, 442, 14, 22, 255, 0, 0, 0);
        state = ON;
    }

    return 0;
}

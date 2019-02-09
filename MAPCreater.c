#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int IMAGE_WIDTH = 16;
const int IMAGE_HEIGHT = 16;
const int MAGNIFICATION = 2;
const int GRID_SIZE = 32;
int ROW = 15;
int COL = 20;
int OUT_OF_MAP = 10;
int DEFAULT_MAP = 1;
int frame = 0;

int clac_offset(int, int, int *, int *);
int load_mapchip(SDL_Renderer *);
int draw_map(SDL_Renderer *);
int cursor_move(SDL_Event, SDL_Renderer *);
int load_image(SDL_Renderer *, SDL_Texture **, char *);


typedef struct {
    int map_x;
    int map_y;
    int offset_x;
    int offset_y;
} CURSOR;

typedef struct {
    int mapchip_id;
    char mapchip_name[256];
    int movable;
    int change_locate;
    SDL_Texture *map_image;
} MAPCHIP;

CURSOR cursor = {1, 1, 0, 0};
MAPCHIP mapchip[256] = {0};

int map_array[65536] = {0};
int number_of_map_image = 0;


int main (int argc, char *argv[]) {

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    window = SDL_CreateWindow( "MAP CREATER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    } else {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    load_mapchip(renderer);

    // main loop
    while (1) {

        SDL_Event e;

        clac_offset(cursor.map_x, cursor.map_y, &cursor.offset_x, &cursor.offset_y);

        SDL_RenderClear(renderer);
        draw_map(renderer);
        cursor_move(e, renderer);
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

    SDL_Quit();

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


int cursor_move(SDL_Event e, SDL_Renderer *renderer) {

    SDL_Rect rectangle;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    rectangle.x = cursor.map_x - cursor.offset_x;
    rectangle.y = cursor.map_y - cursor.offset_y;
    rectangle.w = GRID_SIZE;
    rectangle.h = GRID_SIZE;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_RenderFillRect(renderer, &rectangle);

    if (frame % 12 == 0) {

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP){
            cursor.map_y = cursor.map_y - GRID_SIZE;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN){
            cursor.map_y = cursor.map_y + GRID_SIZE;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT){
            cursor.map_x = cursor.map_x + GRID_SIZE;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT){
            cursor.map_x = cursor.map_x - GRID_SIZE;
        }

     }

    if (frame <= 12000) {
        frame++;
    } else {
        frame = 0;
    }

    return 0;

}

int draw_map(SDL_Renderer *renderer){

    int x, y;
    int start_x = cursor.offset_x / GRID_SIZE - 1;
    int end_x = start_x + SCREEN_WIDTH / GRID_SIZE + 2;
    int start_y = cursor.offset_y / GRID_SIZE - 1;
    int end_y = start_y + SCREEN_HEIGHT/ GRID_SIZE + 2;


    for(y = start_y;y < end_y;y++){
        for(x = start_x; x < end_x;x++){

            SDL_Rect imageRect=(SDL_Rect){0, 0, IMAGE_WIDTH, IMAGE_HEIGHT};
            SDL_Rect drawRect=(SDL_Rect){(x * GRID_SIZE) - cursor.offset_x,
                                         (y * GRID_SIZE) - cursor.offset_y,
                                         IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

            if ((x < 0) || (x > COL - 1) || (y < 0) || (y > ROW - 1)){
                SDL_RenderCopy(renderer, mapchip[OUT_OF_MAP].map_image, &imageRect, &drawRect);
            } else {
                SDL_RenderCopy(renderer, mapchip[DEFAULT_MAP].map_image, &imageRect, &drawRect);
            }

        }
    }

    return 0;
}

int load_mapchip(SDL_Renderer *renderer) {

    FILE *fp;
    int x, y, z;
    char n[256];
    char path[256];
    char buf[256];
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

int clac_offset(int x, int y, int *offset_x, int *offset_y) {
    *offset_x = x - (SCREEN_WIDTH / 2);
    *offset_y = y - (SCREEN_HEIGHT / 2);

    return 0;
}



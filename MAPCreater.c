#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int IMAGE_WIDTH = 16;
const int IMAGE_HEIGHT = 16;
const int MAGNIFICATION = 2;
const int GRID_SIZE = 32;
const int PALLET_ROW = 15;
const int PALLET_COL = 20;

int ROW = 15;
int COL = 20;
int OUT_OF_MAP = 10;
int frame = 0;

int clac_offset(int, int, int *, int *);
int load_mapchip(SDL_Renderer *);
int draw_map(SDL_Renderer *);
int draw_pallet(SDL_Renderer *);
int cursor_move(SDL_Event, SDL_Renderer *);
int load_image(SDL_Renderer *, SDL_Texture **, char *);
int place_mapchip(SDL_Point, SDL_Renderer *);
int place_mapchip_with_key(SDL_Renderer *);
int get_mapchip(SDL_Point, SDL_Renderer *);
int get_mapchip_with_key(SDL_Renderer *);
int initialize(SDL_Renderer *);

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
CURSOR pallet_cursor = {1, 1, 0, 0};
MAPCHIP mapchip[256] = {0};

int *map_array;
int *pallet_array;
int number_of_map_image = 0;
int select_mapchip = 0;


int main (int argc, char *argv[]) {

    int IMAGE_WIDTH_SENTER = IMAGE_WIDTH * MAGNIFICATION / 2;
    int IMAGE_HEIGHT_SENTER = IMAGE_HEIGHT * MAGNIFICATION / 2;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // ウィンドウの位置
    SDL_Point window_position = {
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED
    };

    // ウィンドウのサイズ
    SDL_Point window_size = {SCREEN_WIDTH, SCREEN_HEIGHT};

    // マウスの座標
    SDL_Point mouse_position = {IMAGE_WIDTH_SENTER, IMAGE_WIDTH_SENTER};

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

    initialize(renderer);

    int pallet_display = 1;

    // main loop
    while (1) {

        SDL_Event e;

        clac_offset(cursor.map_x, cursor.map_y, &cursor.offset_x, &cursor.offset_y);
        clac_offset(pallet_cursor.map_x, pallet_cursor.map_y, &pallet_cursor.offset_x, &pallet_cursor.offset_y);

        SDL_RenderClear(renderer);
        if (pallet_display == 0) {
            draw_pallet(renderer);
            pallet_move(e, renderer);
        } else {
            draw_map(renderer);
            cursor_move(e, renderer);
        }
        SDL_RenderPresent(renderer);

        // event handling
        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_QUIT) {
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                break;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_i) {
                if (pallet_display == 0) {
                    pallet_display = 1;
                } else {
                    pallet_display = 0;
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT &&
                         pallet_display == 0) {
                get_mapchip(mouse_position, renderer);
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE && pallet_display == 0) {
                get_mapchip_with_key(renderer);
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT &&
                         pallet_display == 1) {
                place_mapchip(mouse_position, renderer);
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE && pallet_display == 1) {
                place_mapchip_with_key(renderer);
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT){
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

int initialize(SDL_Renderer *renderer) {


    map_array = malloc(sizeof(int) * COL * ROW);
    pallet_array = malloc(sizeof(int) * PALLET_COL * PALLET_ROW);

    load_mapchip(renderer);

    int i;
    for (i=0;i <= COL*ROW;i++) {
        map_array[i] = 1;
    }

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


int place_mapchip(SDL_Point mouse_position, SDL_Renderer *renderer) {

    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
    map_array[((cursor.offset_y + mouse_position.y) / GRID_SIZE)*COL +
              ((cursor.offset_x + mouse_position.x) / GRID_SIZE)] = select_mapchip;

    return 0;

}

int place_mapchip_with_key(SDL_Renderer *renderer) {

    map_array[(cursor.map_y / GRID_SIZE) * COL +
              (cursor.map_x / GRID_SIZE)] = select_mapchip;

    return 0;

}

int get_mapchip(SDL_Point mouse_position, SDL_Renderer *renderer) {

    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
    select_mapchip =
       pallet_array[((pallet_cursor.offset_y + mouse_position.y) / GRID_SIZE)*PALLET_COL +
                    ((pallet_cursor.offset_x + mouse_position.x) / GRID_SIZE)];

    return 0;

}

int get_mapchip_with_key(SDL_Renderer *renderer) {

    select_mapchip =
       pallet_array[(pallet_cursor.map_y / GRID_SIZE) * PALLET_COL +
                    (pallet_cursor.map_x / GRID_SIZE)];

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

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_k){
        cursor.map_y = cursor.map_y - GRID_SIZE;
    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_j){
        cursor.map_y = cursor.map_y + GRID_SIZE;
    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_l){
        cursor.map_x = cursor.map_x + GRID_SIZE;
    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_h){
        cursor.map_x = cursor.map_x - GRID_SIZE;
    }

    return 0;

}

int pallet_move(SDL_Event e, SDL_Renderer *renderer) {

    SDL_Rect rectangle;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    rectangle.x = pallet_cursor.map_x - pallet_cursor.offset_x;
    rectangle.y = pallet_cursor.map_y - pallet_cursor.offset_y;
    rectangle.w = GRID_SIZE;
    rectangle.h = GRID_SIZE;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_RenderFillRect(renderer, &rectangle);

    if (frame % 12 == 0) {

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP){
            pallet_cursor.map_y = pallet_cursor.map_y - GRID_SIZE;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN){
            pallet_cursor.map_y = pallet_cursor.map_y + GRID_SIZE;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT){
            pallet_cursor.map_x = pallet_cursor.map_x + GRID_SIZE;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT){
            pallet_cursor.map_x = pallet_cursor.map_x - GRID_SIZE;
        }

     }

    if (frame <= 12000) {
        frame++;
    } else {
        frame = 0;
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_k){
        pallet_cursor.map_y = pallet_cursor.map_y - GRID_SIZE;
    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_j){
        pallet_cursor.map_y = pallet_cursor.map_y + GRID_SIZE;
    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_l){
        pallet_cursor.map_x = pallet_cursor.map_x + GRID_SIZE;
    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_h){
        pallet_cursor.map_x = pallet_cursor.map_x - GRID_SIZE;
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
                SDL_RenderCopy(renderer, mapchip[map_array[y*COL+x]].map_image, &imageRect, &drawRect);
            }

        }
    }

    return 0;
}

int draw_pallet(SDL_Renderer *renderer){

    int x, y;
    int start_x = pallet_cursor.offset_x / GRID_SIZE - 1;
    int end_x = start_x + SCREEN_WIDTH / GRID_SIZE + 2;
    int start_y = pallet_cursor.offset_y / GRID_SIZE - 1;
    int end_y = start_y + SCREEN_HEIGHT/ GRID_SIZE + 2;


    for(y = start_y;y < end_y;y++){
        for(x = start_x; x < end_x;x++){

            SDL_Rect imageRect=(SDL_Rect){0, 0, IMAGE_WIDTH, IMAGE_HEIGHT};
            SDL_Rect drawRect=(SDL_Rect){(x * GRID_SIZE) - pallet_cursor.offset_x,
                                         (y * GRID_SIZE) - pallet_cursor.offset_y,
                                         IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

            if ((x < 0) || (x > PALLET_COL - 1) || (y < 0) || (y > PALLET_ROW - 1)){
                SDL_RenderCopy(renderer, mapchip[OUT_OF_MAP].map_image, &imageRect, &drawRect);
            } else {
                if(number_of_map_image >= y*PALLET_COL+x) {
                    SDL_RenderCopy(renderer, mapchip[pallet_array[y*PALLET_COL+x]].map_image, &imageRect, &drawRect);
                } else {
                    SDL_RenderCopy(renderer, mapchip[OUT_OF_MAP].map_image, &imageRect, &drawRect);
                }
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

        pallet_array[i] = x;
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



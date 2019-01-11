#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int IMAGE_WIDTH = 16;
const int IMAGE_HEIGHT = 16;      
const int MAGNIFICATION = 2;
const int ROW = 15;
const int COL = 20;
const int GRID_SIZE = 32;
const int NUMBER_OF_MAP_IMAGE = 3;
int animecycle = 24;
int speed = 2;
int frame = 0;

typedef enum {DOWN, LEFT, RIGHT, UP} DIRECTION;
typedef enum {FALSE, TRUE} MOVING;

typedef struct {
    int map_x;
    int map_y;
    int pixel_x;
    int pixel_y;
    int offset_x;
    int offset_y;
    int velocity_x;
    int velocity_y;
    DIRECTION direction;
    MOVING moving;
} Caracter;

typedef struct {
    int mapchip_id;
    char *mapchip_name;
    int movable;
    int transparentColor;
} Map;

Caracter player = {1, 1, 32, 32, 0, 0, 0, 0, DOWN, FALSE};

int load_image(SDL_Renderer *, SDL_Texture **, char *);
int character_animation(SDL_Renderer *, SDL_Event);
int character_move(SDL_Event);

int load_map_image(SDL_Renderer *, SDL_Texture **);
int load_map(char *);
int draw_map(SDL_Renderer *, SDL_Texture **);
int is_movable(int, int);
int clac_offset(int, int, int *, int *);

int map[10000];
// int map[300] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};


int main (int argc, char *argv[]) {

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    window = SDL_CreateWindow( "DRAW IMAGE TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
	return 1;
    } else {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    // Map map[get_map_num("data/mapchip.dat")];
    SDL_Texture *map_image[NUMBER_OF_MAP_IMAGE];
    load_map_image(renderer, map_image);
    load_map("data/field.map");

    // main loop
    while (1) {
        SDL_Event e;

        clac_offset(player.pixel_x, player.pixel_y, &player.offset_x, &player.offset_y);

        SDL_RenderClear(renderer);
        draw_map(renderer, map_image);
        character_animation(renderer, e);
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
    for (i = 0;i < NUMBER_OF_MAP_IMAGE;i++) {
        SDL_DestroyTexture(map_image[i]);
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

int character_animation(SDL_Renderer *renderer, SDL_Event e) {

    SDL_Texture *cat_image = NULL;
    load_image(renderer, &cat_image, "image/charachip/black_cat.bmp");

    if (player.moving == TRUE) {
        player.pixel_x = player.pixel_x + player.velocity_x;
        player.pixel_y = player.pixel_y + player.velocity_y;

        if (player.pixel_x % GRID_SIZE == 0 && player.pixel_y % GRID_SIZE == 0){
            player.moving = FALSE;
            player.map_x = player.pixel_x / GRID_SIZE;
            player.map_y = player.pixel_y / GRID_SIZE;

            if (map[player.map_y*COL+player.map_x] == 3){
                load_map("data/field2.map");
            }

	    character_move(e);
        }

    } else {
	character_move(e);
    }

    int x = ((frame / animecycle) % 4) * 16;    
    int y = player.direction * IMAGE_HEIGHT;

    SDL_Rect imageRect=(SDL_Rect){x, y, IMAGE_WIDTH, IMAGE_HEIGHT};      
    SDL_Rect drawRect=(SDL_Rect){player.pixel_x - player.offset_x, player.pixel_y - player.offset_y,
	                         IMAGE_WIDTH*MAGNIFICATION, IMAGE_HEIGHT*MAGNIFICATION};

    SDL_RenderCopy(renderer, cat_image, &imageRect, &drawRect);

    if (frame <= animecycle * 4) {
        frame += 1;
    } else{
        frame = 0;
    }

    SDL_DestroyTexture(cat_image);

    return 0;
}

int character_move(SDL_Event e) {

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
    
    return 0;

}

int draw_map(SDL_Renderer *renderer, SDL_Texture **map_image){

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
                SDL_RenderCopy(renderer, map_image[1], &imageRect, &drawRect);
	    } else {
                SDL_RenderCopy(renderer, map_image[map[y*COL+x]], &imageRect, &drawRect);
            }

        }
    }

    return 0;
}

int load_map_image(SDL_Renderer *renderer, SDL_Texture **map_image){

    load_image(renderer, &map_image[0], "image/mapchip/grass.bmp");
    load_image(renderer, &map_image[1], "image/mapchip/water.bmp");
    load_image(renderer, &map_image[2], "image/mapchip/mountain.bmp");

    return 0;
}


int load_map(char *map_name) {

    FILE *fp;
    int map_num;

    int i = 0;
    if((fp=fopen(map_name, "r")) != NULL){
        while((map_num = fgetc(fp)) != EOF){
            if(map_num != 0x0d){
                if(map_num != 0x0a){
                    map[i] = map_num - 48;
                    i++;
                }
	    }
        }
    }

    return 0;
}

int is_movable(int x, int y) {

    if ( x < 0 || x > COL - 1 || y  < 0 || y > ROW - 1) {
        return 1;
    }

    if (map[y*COL+x] == 1 || map[y*COL+x] == 2){
        return 1;
    }

    return 0;
}

int clac_offset(int x, int y, int *offset_x, int *offset_y) {
    *offset_x = x - (SCREEN_WIDTH / 2);
    *offset_y = y - (SCREEN_HEIGHT / 2);

    return 0;
}

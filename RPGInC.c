#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "RPGInC.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int IMAGE_WIDTH = 16;
const int IMAGE_HEIGHT = 16;      
const int MAGNIFICATION = 2;
const int GRID_SIZE = 32;
const int NUMBER_OF_MAP_IMAGE = 5;
int ROW = 15;
int COL = 20;
int OUT_OF_MAP = 0;
char MAP_EVENT_NAME[256] = "field";

int animecycle = 24;
int speed = 2;
int frame = 0;

CARACTER player = {1, 1, 32, 32, 0, 0, 0, 0, DOWN, FALSE};
MAPCHIP mapchip[256] = {0};

int map_array[65536] = {0};


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

    mapchip_load(renderer);
    load_map("data/field.map");

    // main loop
    while (1) {
        SDL_Event e;

        clac_offset(player.pixel_x, player.pixel_y, &player.offset_x, &player.offset_y);

        SDL_RenderClear(renderer);
        draw_map(renderer);
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

            load_event();
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

int load_event(void) {
    char event_path[256];

    sprintf(event_path, "data/%s.evt", MAP_EVENT_NAME);

    FILE *fp;
    char event[256];
    int event_point_x;
    int event_point_y;
    DIRECTION direction_of_penetration;
    char buf[256];
    char new_map_name[256];
    char map_path[256];
    int new_x;
    int new_y;
    int i = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {
        sscanf(buf,
	       "%[^,],%d,%d,%d,%[^,],%d,%d",
               event, &event_point_x, &event_point_y, &direction_of_penetration, new_map_name, &new_x, &new_y);
		
	// printf("------------------------------\n");
	// printf("event:%s\n", event);
	// printf("p_x:%d\n", event_point_x);
	// printf("p_y:%d\n", event_point_y);
	// printf("direction:%d\n", direction_of_penetration);
	// printf("new_map:%s\n", new_map_name);
	// printf("n_x:%d\n", new_x);
	// printf("n_y:%d\n", new_y);

	if (strcmp(event,"MOVE") == 0) {
            if (player.map_x == event_point_x && player.map_y == event_point_y) {
                sprintf(MAP_EVENT_NAME, "%s", new_map_name);

                sprintf(map_path, "data/%s.map", new_map_name);
		load_map(map_path);

                player.map_x = new_x;
                player.map_y = new_y;
                player.pixel_x = player.map_x * GRID_SIZE;
                player.pixel_y = player.map_y * GRID_SIZE;
		player.direction = direction_of_penetration;
            }	    
	}
    }

    fclose(fp);

    return 0;
}

int load_map(char *map_name) {
    FILE *fp;
    int map_num;

    fp = fopen(map_name, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    fscanf(fp, "%d%d", &COL, &ROW);
    fscanf(fp, "%d", &OUT_OF_MAP);

    int i = 0;
    while((map_num = fgetc(fp)) != EOF){
        if(map_num != 0x0d){
            if(map_num != 0x0a){
                map_array[i] = map_num - 48;
                i++;
            }
       }
    }

    
    return 0;
}

int mapchip_load(SDL_Renderer *renderer) {

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

    fclose(fp);

    return 0;
    
}

int is_movable(int x, int y) {

    if ( x < 0 || x > COL - 1 || y  < 0 || y > ROW - 1) {
        return 1;
    }

    if(mapchip[map_array[y*COL+x]].movable == 1){
        return 1;
    }

    return 0;
}

int clac_offset(int x, int y, int *offset_x, int *offset_y) {
    *offset_x = x - (SCREEN_WIDTH / 2);
    *offset_y = y - (SCREEN_HEIGHT / 2);

    return 0;
}



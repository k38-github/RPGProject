#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "load.h"

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

int load_mapchip(SDL_Renderer *renderer, MAPCHIP *mapchip, int number_of_map_image) {

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

int load_map(char *map_name, int *map_array, int *col, int *row, int *out_of_map) {
    FILE *fp;
    int *tmp;
    int i = 0;

    if ((fp = fopen(map_name, "rb")) == NULL) {
        return 1;
    }

    fread(col, sizeof(int), 1, fp);
    fread(row, sizeof(int), 1, fp);
    fread(out_of_map, sizeof(int), 1, fp);

    while(!feof(fp)) {
        fread(&map_array[i++], sizeof(int), 1, fp);
    }

    fclose(fp);

    return 0;
}


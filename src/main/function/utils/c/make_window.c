#include <stdio.h>
#include <SDL2/SDL.h>
#include "../headers/make_window.h"
#include "../../parts.h"

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



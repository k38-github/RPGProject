#include <stdio.h>
#include <SDL2/SDL.h>
#include "../headers/flash_triangle.h"
#include "../headers/make_triangle.h"
#include "../../parts.h"


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

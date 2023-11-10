#include <stdio.h>
#include <SDL2/SDL.h>
#include "../headers/make_triangle.h"
#include "../../parts.h"

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



#include <stdio.h>
#include "../headers/fade_out.h"
#include "../headers/calc_offset.h"
#include "../../draw/headers/draw_map.h"
#include "../../player/headers/player_animation.h"
#include "../../parts.h"

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
            calc_offset(player.pixel_x, player.pixel_y, &player.offset_x, &player.offset_y);
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



#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../sounds/sounds.h"
#include "../load/load.h"


int damage_flush(SDL_Renderer *renderer, SDL_Texture **image_texture,
                 SDL_Rect imageRect, SDL_Rect drawRect) {

    int i;

    SDL_SetTextureBlendMode(*image_texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(*image_texture, 255);

    sound_se("damage.ogg");

    for (i=0;i<13;i++) {
        SDL_Delay(55);

        if ((i % 2) == 0) {
            SDL_SetTextureColorMod(*image_texture, 255, 255, 255);
        } else {
            SDL_SetTextureColorMod(*image_texture, 0, 0, 0);
        }

        SDL_RenderCopy(renderer, *image_texture, &imageRect, &drawRect);
        SDL_RenderPresent(renderer);
    }

    return 0;

}

int blow_effects(SDL_Renderer *renderer, int image_w, int image_h, int draw_x, int draw_y) {

    int i, counter;

    SDL_Texture *blow_image = NULL;

    load_image(renderer, &blow_image, "image/effects/blow.bmp");

    SDL_Rect imageRect = (SDL_Rect){0, 0, image_w, image_h};
    SDL_Rect drawRect = (SDL_Rect){draw_x-image_w/3, draw_y-image_h/3, image_w, image_h};

    SDL_SetTextureBlendMode(blow_image, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(blow_image, 255);


    for (counter=0;counter<3;counter++) {

        sound_se("blow.ogg");

        if (counter == 1) {
            drawRect.x = draw_x+image_w/3;
            drawRect.y = draw_y;
        } else if (counter == 2) {
            drawRect.x = draw_x-image_w/3;
            drawRect.y = draw_y+image_h/3;
        }

        for (i=0;i<7;i++) {
            SDL_Delay(10);

            if ((i % 2) == 0) {
                SDL_SetTextureColorMod(blow_image, 255, 255, 255);
            } else {
                SDL_SetTextureColorMod(blow_image, 0, 0, 0);
            }

            SDL_RenderCopy(renderer, blow_image, &imageRect, &drawRect);
            SDL_RenderPresent(renderer);
        }

    }

    SDL_Delay(350);

    for (counter=0;counter<3;counter++) {

        if (counter == 0) {
            drawRect.x = draw_x-image_w/3;
            drawRect.y = draw_y-image_h/3;
        } else if (counter == 1) {
            drawRect.x = draw_x+image_w/3;
            drawRect.y = draw_y;
        } else if (counter == 2) {
            drawRect.x = draw_x-image_w/3;
            drawRect.y = draw_y+image_h/3;
        }

        SDL_SetTextureColorMod(blow_image, 0, 0, 0);

        SDL_RenderCopy(renderer, blow_image, &imageRect, &drawRect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(blow_image);

    return 0;
}

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


int damage_flush(SDL_Renderer *renderer, SDL_Texture **image_texture,
                 SDL_Rect imageRect, SDL_Rect drawRect) {

    int i;

    SDL_SetTextureBlendMode(*image_texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(*image_texture, 255);

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

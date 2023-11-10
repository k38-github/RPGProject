#include <stdio.h>
#include "../headers/draw_monster.h"
#include "../../load/headers/load.h"
#include "../../parts.h"

int draw_monster(SDL_Renderer *renderer, char *path, int num_of_monster, MONSTER *monster) {

    int i;
    int pos;
    int w, h;

    SDL_Texture *monster_image = NULL;
    load_image(renderer, &monster_image, path);

    SDL_QueryTexture(monster_image, NULL, NULL, &w, &h);

    SDL_Rect imageRect;
    SDL_Rect drawRect;

    if ((num_of_monster % 2) == 1) {
        pos = -1 * w/2 - (w * (num_of_monster / 2)) ;

        for (i=0;i<num_of_monster;i++) {
            if (0 < monster[i].status.hp) {
                imageRect=(SDL_Rect){0, 0, w, h};
                drawRect=(SDL_Rect){SCREEN_WIDTH/2+pos, SCREEN_HEIGHT/2-h/2, w, h};

                SDL_RenderCopy(renderer, monster_image, &imageRect, &drawRect);

                monster[i].monster_image = monster_image;
                monster[i].imageRect = imageRect;
                monster[i].drawRect = drawRect;
            }
            pos = pos + w;
        }
    } else if ((num_of_monster % 2) == 0) {
        pos = -1 * w * (num_of_monster/2);

        for (i=0;i<num_of_monster;i++) {
            if (0 < monster[i].status.hp) {
                imageRect=(SDL_Rect){0, 0, w, h};
                drawRect=(SDL_Rect){SCREEN_WIDTH/2+pos, SCREEN_HEIGHT/2-h/2, w, h};
                SDL_RenderCopy(renderer, monster_image, &imageRect, &drawRect);

                monster[i].monster_image = monster_image;
                monster[i].imageRect = imageRect;
                monster[i].drawRect = drawRect;
            }
            pos = pos + w;
        }
    }

    return 0;

}

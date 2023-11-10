#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../headers/get_message.h"
#include "../headers/make_window.h"
#include "../headers/message_engine.h"
#include "../headers/window_update.h"
#include "../../parts.h"

int window_update(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e) {

    if (message_window.visible == IN_VISIBLE) {
        if (state == OFF) {
            state = ON;
            get_message();
        } else {
            make_window(renderer, message_window);
            message_engine(renderer, font, e);
            state = OFF;
        }
    }

    return 0;
}

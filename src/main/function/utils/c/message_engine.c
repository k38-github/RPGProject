#include <stdio.h>
#include "../headers/make_window.h"
#include "../headers/window_update.h"
#include "../../utils/headers/display_character_string.h"
#include "../../utils/headers/flash_triangle.h"
#include "../../utils/headers/u8mb.h"
#include "../../sounds/headers/sounds.h"
#include "../../parts.h"

int message_engine(SDL_Renderer *renderer, TTF_Font *font, SDL_Event e) {

    int x;
    int message_length;
    int remaining_message_length;
    int word_length = 0;
    int byte_counter;
    int byt;

    int loop_counter = 0;
    int row_position = 0;
    int col_position = 0;
    int tmp_position = 0;

    const int row_size = 21;
    const int col_size = 84;
    const double start_x = 153.0;
    const double start_y = 354.0;
    const double inter_char = 5.35;

    char message_tmp[1024];
    char message_out[100];
    char word[6];
    char isspace[3] = {0};
    char isasterisk[3] = {0};

    char *se_file = "conversation.ogg";

    sound_se(se_file);

    strcpy(message_tmp, message);
    message_length = strlen(message);

    while (*message != '\0') {
        SDL_Delay(60);

        byt = u8mb(*message);
        message += u8mb(*message);

        remaining_message_length = strlen(message);

        memset(word, '\0', 6);

        byte_counter = 0;
        for (x=word_length;x<message_length - remaining_message_length;x++) {
            sprintf(&word[byte_counter], "%c", message_tmp[x]);
            byte_counter++;
        }

        // 改行判定(半角スペースが2つ続いたら改行とみなす)
        if (*word == ' ') {
            if (strcmp(isspace, "  ") == 0) {
                memset(isspace, '\0', 3);
            }
            strncat(isspace, word, 1);
        } else {
            memset(isspace, '\0', 3);
        }

        if (strcmp(isspace, "  ") == 0) {
            if (loop_counter % col_size != 0) {
                loop_counter = loop_counter + (row_size - (loop_counter % row_size) - 1);
            }
        }

        // 改ページ判定(アスタリスクが2つ続いたら改ページとみなす)
        if (*word == '*') {
            if (strcmp(isasterisk, "**") == 0) {
                memset(isasterisk, '\0', 3);
            }
            strncat(isasterisk, word, 1);
            strncpy(word, " ", 1);
        } else {
            memset(isasterisk, '\0', 3);
        }

        if (strcmp(isasterisk, "**") == 0) {
            loop_counter = loop_counter + (col_size - (loop_counter % col_size) - 1);
        }

        // 折り返し判定
        if (loop_counter != 0 && loop_counter % row_size == 0) {
            row_position += row_size;
            col_position = 0;
            tmp_position = word_length;
            if (loop_counter % col_size == 0 ) {
                while (1) {
                    flash_triangle(renderer);

                    if ( SDL_PollEvent(&e) ) {
                        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                            sound_se(se_file);
                            break;
                        }
                    }
                }
                make_window(renderer, message_window);
                row_position = 0;
                col_position = 0;
            } else {
                sound_se(se_file);
            }
        } else if (loop_counter % row_size > 0) {
            col_position = word_length - tmp_position;
        }

        display_character_string(renderer, font, word, start_x + col_position * inter_char, start_y + row_position, 0);

        word_length = message_length - remaining_message_length;

        if ( SDL_PollEvent(&e) ) {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                col_position = col_position + byt;
                loop_counter++;

                memset(message_out, '\0', 100);

                while (*message != '\0') {
                    byt = u8mb(*message);
                    message += u8mb(*message);

                    remaining_message_length = strlen(message);

                    memset(word, '\0', 6);

                    byte_counter = 0;
                    for (x=word_length;x<message_length - remaining_message_length;x++) {
                        sprintf(&word[byte_counter], "%c", message_tmp[x]);
                        byte_counter++;
                    }

                    // 改行判定(半角スペースが2つ続いたら改行とみなす)
                    if (*word == ' ') {
                        if (strcmp(isspace, "  ") == 0) {
                            memset(isspace, '\0', 3);
                        }
                        strncat(isspace, word, 1);
                    } else {
                        memset(isspace, '\0', 3);
                    }

                    if (strcmp(isspace, "  ") == 0) {
                        if (loop_counter % col_size != 0) {
                            loop_counter = loop_counter + (row_size - (loop_counter % row_size) - 1);
                        }
                    }

                    // 改ページ判定(アスタリスクが2つ続いたら改ページとみなす)
                    if (*word == '*') {
                        if (strcmp(isasterisk, "**") == 0) {
                            memset(isasterisk, '\0', 3);
                        }
                        strncat(isasterisk, word, 1);
                        strncpy(word, " ", 1);
                    } else {
                        memset(isasterisk, '\0', 3);
                    }

                    if (strcmp(isasterisk, "**") == 0) {
                        loop_counter = loop_counter + (col_size - (loop_counter % col_size) - 1);
                    }

                    // 折り返し判定
                    if (loop_counter % row_size == 0) {
                        display_character_string(renderer, font, message_out, start_x + col_position * inter_char, start_y + row_position, 0);
                        row_position += row_size;
                        col_position = 0;
                        tmp_position = word_length;
                        memset(message_out, '\0', 100);

                        if (loop_counter % col_size == 0 ) {
                            while (1) {
                                flash_triangle(renderer);

                                if ( SDL_PollEvent(&e) ) {
                                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                                        break;
                                    }
                                }
                            }
                            make_window(renderer, message_window);
                            row_position = 0;
                            col_position = 0;
                        }
                    }

                    strcat(message_out, word);
                    word_length = message_length - remaining_message_length;

                    loop_counter++;
                }
                display_character_string(renderer, font, message_out, start_x + col_position * inter_char, start_y + row_position, 0);

                break;
            }
        }

        loop_counter++;
    }

    if (flash_triangle_status == ON) {
        if (message_window.visible == IN_VISIBLE) {
            while (1) {
                flash_triangle(renderer);

                if ( SDL_PollEvent(&e) ) {
                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                        message_window.visible = OUT_VISIBLE;
                        break;
                    }
                }
            }
        }
    } else {
        SDL_Delay(300);
        message_window.visible = OUT_VISIBLE;
    }

    return 0;
}



#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

Mix_Music *sounds = NULL;

int initialize_sounds() {
    // Initialize Mixer
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf( "Mixer could not initialize! Mixer_Error: %s\n", Mix_GetError());
        return 1;
    }
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
        printf( "Mix_OpenAudio could not initialize! Mixer_Error: %s\n", Mix_GetError());
        return 1;
    }
    //

    sounds = Mix_LoadMUS("sounds/bgm/opening.ogg");
    if (sounds == NULL) {
        return 1;
    }
    if (Mix_PlayMusic(sounds, -1) == -1) {
        return 1;
    }

    Mix_VolumeMusic(10);
    Mix_PlayingMusic();

    return 0;
}

int quit_sounds() {
    // quit SDL_mixer
    Mix_FreeMusic(sounds);
    Mix_CloseAudio();

    return 0;
}

int pause_sounds() {
    Mix_PauseMusic();

    return 0;
}

int resume_sounds() {
    Mix_ResumeMusic();

    return 0;
}

int load_se(int player_map_x, int player_map_y, char *map_event_name) {

    char event_path[256] = {0};

    sprintf(event_path, "data/%s.evt", map_event_name);

    FILE *fp;
    char buf[256] = {0};
    char event[256] = {0};
    int event_point_x;
    int event_point_y;
    char *se_name;
    char se_path[256] = {0};

    int i = 0;

    Mix_Chunk *wave = NULL;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    if ((se_name = malloc(sizeof(char) * 256)) == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "SE", 2) == 0) {
                sscanf(buf,
                   "%[^,],%d,%d,%[^,]",
                       event, &event_point_x, &event_point_y, se_name);

                if (player_map_x == event_point_x && player_map_y == event_point_y) {
                    sprintf(se_path, "sounds/se/%s", se_name);

                    wave = Mix_LoadWAV(se_path);
                    if (wave == NULL) {
                        return 1;
                    }

                    Mix_VolumeChunk(wave, 10);
                    if ( Mix_PlayChannel(-1, wave, 0) == -1 ) {
                        return 1;
                    }

                    break;
                }
            }
        }
    }

    fclose(fp);
    free(se_name);

    return 0;
}

int sound_se(char *se_name) {

    Mix_Chunk *wave = NULL;
    char se_path[256] = {0};

    sprintf(se_path, "sounds/se/%s", se_name);

    wave = Mix_LoadWAV(se_path);
    if (wave == NULL) {
        return 1;
    }

    Mix_VolumeChunk(wave, 10);
    if ( Mix_PlayChannel(-1, wave, 0) == -1 ) {
        return 1;
    }

    return 0;
}

int load_bgm(char *map_event_name) {
    char event_path[256] = {0};

    sprintf(event_path, "data/%s.evt", map_event_name);

    FILE *fp;
    char buf[256] = {0};
    char event[256] = {0};
    char *bgm_name;
    char bgm_path[256] = {0};

    int i = 0;

    fp = fopen(event_path, "r");
    if (fp == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }

    if ((bgm_name = malloc(sizeof(char) * 256)) == NULL) {
        printf("file open error. %d\n", __LINE__);
        return 1;
    }


    for(i = 0;fgets(buf, sizeof(buf), fp) != NULL;i++) {

        if (strncmp(buf, "#", 1) != 0){
            if (strncmp(buf, "BGM", 3) == 0) {
                sscanf(buf,
                   "%[^,],%[^,]",
                       event, bgm_name);

                sprintf(bgm_path, "sounds/bgm/%s", bgm_name);

                sounds = Mix_LoadMUS(bgm_path);
                if (sounds == NULL) {
                    return 1;
                }
                if (Mix_PlayMusic(sounds, -1) == -1) {
                    return 1;
                }

                break;
            }
        }
    }

    fclose(fp);
    free(bgm_name);

    return 0;
}

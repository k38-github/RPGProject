#ifdef _SOUNDS
#else

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

int initialize_sounds(void);
int load_se(int, int, char *);
int sound_se(char *);
int load_bgm(char *);
int quit_sounds(void);
int pause_sounds(void);
int resume_sounds(void);

#endif



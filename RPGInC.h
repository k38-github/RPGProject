#ifdef _RPGINC
#else

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

typedef enum {DOWN, LEFT, RIGHT, UP} DIRECTION;
typedef enum {FALSE, TRUE} MOVING;
typedef enum {OUT_VISIBLE, IN_VISIBLE} VISIBLE;
typedef enum {OFF, ON} STATE;
typedef enum {TALK, MEMORY, STATUS, EQUIPMENT, OPEN, SPELL, SKILL, TOOLS, TACTICS, SEARCH} COMMAND_STATUS;
typedef enum {HP_AND_MP, OFFENSIVE_POWER, CONFIRM_STATUS} STATUS_STATUS;


typedef struct {
    char *name;
    int   sex;
    int   level;
    int   hp;
    int   mp;
    int   strength;
    int   agility;
    int   protection;
    int   wisdom;
    int   luck;
    int   max_hp;
    int   max_mp;
    int   offensive_power;
    int   defensive_power;
    int   skill_point;
    int   experience_point;
} CHARACTER_STATUS;

typedef struct {
    int map_x;
    int map_y;
    int pixel_x;
    int pixel_y;
    int offset_x;
    int offset_y;
    int velocity_x;
    int velocity_y;
    DIRECTION direction;
    MOVING moving;
    CHARACTER_STATUS status;
} CARACTER;

typedef struct {
    CARACTER npc;
    char message[1024];
    SDL_Texture *npc_image;
    MOVING npc_move;
} NPC;

typedef struct {
    int mapchip_id;
    char mapchip_name[256];
    int movable;
    int change_locate;
    SDL_Texture *map_image;
} MAPCHIP;

typedef struct {
    int rectangle_x;
    int rectangle_y;
    int rectangle_w;
    int rectangle_h;
    int blend;
    VISIBLE visible;
} WINDOW;

typedef struct {
    int map_x;
    int map_y;
    char item[128];
    int status;
} TREASURE;

typedef struct {
    char map[256];
    TREASURE treasure[256];
} TREASURE_FRAME;

typedef struct {
    int map_x;
    int map_y;
    int status;
} DOOR;

int clac_offset(int, int, int *, int *);
int load_image(SDL_Renderer *, SDL_Texture **, char *);
int player_animation(SDL_Renderer *, SDL_Texture *);
int player_update(SDL_Renderer *, SDL_Event, SDL_Texture *);
int player_move(SDL_Event);
int set_player_status();

int load_npc(SDL_Renderer *);
int npc_animation(SDL_Renderer *);
int npc_update(SDL_Renderer *renderer, int);
int npc_move(DIRECTION, int);

int load_map_image(SDL_Renderer *, SDL_Texture **);
int load_mapchip(SDL_Renderer *);
int load_move(SDL_Renderer *, SDL_Texture *);
int load_se(void);
int load_bgm(void);
int sound_se(char *);
int load_map(char *);
int draw_map(SDL_Renderer *);
int load_treasure(SDL_Renderer *);
int draw_treasure(SDL_Renderer *);
int load_door(SDL_Renderer *);
int draw_door(SDL_Renderer *, SDL_Event);
int open_door(void);
int is_movable(int, int);
int fade_out(SDL_Renderer *, SDL_Texture *);

int make_window(SDL_Renderer *, WINDOW);
int make_box(SDL_Renderer *, int, int, int, int, int, int, int, int);
int make_triangle(SDL_Renderer *, int, int, int, int, int, int, int, int, int, int, int);
int window_update(SDL_Renderer *, TTF_Font *, SDL_Event);
int message_window_status();
int message_engine(SDL_Renderer *, TTF_Font *, SDL_Event);
int display_character_string(SDL_Renderer *, TTF_Font *, char *, double, double, int);
int get_message();
int get_treasure_message();
int get_npc_message();

int u8mb(const char);
int flash_triangle(SDL_Renderer *);

int draw_debug_info(SDL_Renderer *, TTF_Font *);

int make_commands_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int check_command_status(COMMAND_STATUS *, int, int);
int get_command_triangle(int *, int *, int *, int *, int *, int *);

int make_status_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int check_status_status(STATUS_STATUS *, int, int);
int get_status_triangle(int *, int *, int *, int *, int *, int *);

int make_hp_and_mp_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int convert_int_to_full_width_char(int, char *);

/*** message list ***/
#define TALK_MESSAGE "そっちには　だれも　いないよ。"
#define SEARCH_MESSAGE "あしもとを　しらべたけど　なにもないよ。"
#define TRESUREBOX_MESSAGE "　をてにいれた！"
#define TRESUREBOX_EMPTY_MESSAGE "からっぽ！"
#define DOOR_MESSAGE "そのほうこうに　とびらは　ないよ!"

/*** Initialize value ***/

#define FONT_PATH "font/PixelMplus12-Regular.ttf"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;
TTF_Font *title_font = NULL;
TTF_Font *main_title_font = NULL;
Mix_Music *music = NULL;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int IMAGE_WIDTH = 16;
const int IMAGE_HEIGHT = 16;
const int MAGNIFICATION = 2;
const int GRID_SIZE = 32;
const int FONT_SIZE = 16;
int ROW = 15;
int COL = 20;
int OUT_OF_MAP = 0;
char MAP_EVENT_NAME[256] = "field";

int animecycle = 24;
int speed = 2;
int frame = 0;
int number_of_map_image = 0;
int number_of_npc_image = 0;

CARACTER player = {1, 1, 32, 32, 0, 0, 0, 0, DOWN, FALSE};
NPC npc[256] = {0};

TREASURE_FRAME treasure[256] = {0};
DOOR door[256] = {0};

MAPCHIP mapchip[256] = {0};

int *map_array;

WINDOW message_window = {140, 334, 360, 140, 255, OUT_VISIBLE};
WINDOW debug_window = {5, 5, 100, 50, 255, OUT_VISIBLE};
WINDOW command_window = {16, 16, 216, 160, 255, OUT_VISIBLE};
WINDOW status_window = {32, 32, 170, 135, 255, OUT_VISIBLE};
WINDOW hp_and_mp_window = {48, 48, 170, 220, 255, OUT_VISIBLE};

char *message = SEARCH_MESSAGE;

STATE state = OFF;
STATE debug_state = OFF;
COMMAND_STATUS command_status = TALK;
STATUS_STATUS status_status = HP_AND_MP;

int back_flg = 0;

#endif

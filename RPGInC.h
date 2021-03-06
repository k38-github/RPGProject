#ifdef _RPGINC
#else

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef enum {DOWN, LEFT, RIGHT, UP} DIRECTION;
typedef enum {FALSE, TRUE} MOVING;
typedef enum {OUT_VISIBLE, IN_VISIBLE} VISIBLE;
typedef enum {OFF, ON} STATE;
typedef enum {TALK, MEMORY, STATUS, EQUIPMENT, OPEN, SPELL, SKILL, TOOLS, TACTICS, SEARCH} COMMAND_STATUS;
typedef enum {HP_AND_MP, OFFENSIVE_POWER, CONFIRM_STATUS} STATUS_STATUS;
typedef enum {NORMAL, BATTLE_SELECT, BATTLE_ACTION, ESCAPE, ATTACK, DEFENSE, ITEM, BATTLE_END} BATTLE_STATUS;
typedef enum {ENEMY_POS_1, ENEMY_POS_2, ENEMY_POS_3, ENEMY_POS_4, ENEMY_POS_5} ENEMY_POS;


typedef struct {
    char name[256];
    int sex;
    int level;
    int hp;
    int mp;
    int strength;
    int agility;
    int protection;
    int wisdom;
    int luck;
    int max_hp;
    int max_mp;
    int offensive_power;
    int defensive_power;
    int skill_point;
    int experience_point;
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
} CHARACTER;

typedef struct {
    CHARACTER npc;
    char message[1024];
    SDL_Texture *npc_image;
    MOVING npc_move;
} NPC;

typedef struct {
// ID, モンスター名, 性別, レベル, HP, MP, 攻撃力, 素早さ, 防御力, 賢さ, 運
// スキル1, スキル2, スキル3, スキル4, スキル5
// アイテム1, 確率, アイテム2, 確率, アイテム3, 確率, 取得ゴールド, 取得経験値, 出現確率

    int monster_id;
    char skill1[256];
    char skill2[256];
    char skill3[256];
    char skill4[256];
    char skill5[256];
    char item1[256];
    double item1_probability;
    char item2[256];
    double item2_probability;
    char item3[256];
    double item3_probability;
    int gold;
    double encounter_probability;
    CHARACTER_STATUS status;

    SDL_Texture *monster_image;
    SDL_Rect imageRect;
    SDL_Rect drawRect;

} MONSTER;

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
int player_animation(SDL_Renderer *, SDL_Texture *);
int player_update(SDL_Renderer *, SDL_Event, SDL_Texture *);
int player_move(SDL_Event);
int set_player_status();

int load_npc(SDL_Renderer *);
int npc_animation(SDL_Renderer *);
int npc_update(SDL_Renderer *renderer, int);
int npc_move(DIRECTION, int);

int load_monster(SDL_Renderer *);
int load_monster_status(char *, int, int);

int battle_window(SDL_Renderer *, SDL_Event, MONSTER);
int battle_encount(SDL_Renderer *, SDL_Event);
int draw_monster(SDL_Renderer *, char *, int, MONSTER *);

int load_map_image(SDL_Renderer *, SDL_Texture **);
int load_move(SDL_Renderer *, SDL_Texture *);
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
int flush_message_engine(SDL_Renderer *, TTF_Font *, SDL_Event);
int display_character_string(SDL_Renderer *, TTF_Font *, char *, double, double, int);
int display_aliging_to_the_right(SDL_Renderer *, TTF_Font *, char *, double, double, int);
int get_message();
int get_treasure_message();
int get_npc_message();

int u8mb(const char);
int flash_triangle(SDL_Renderer *);

int draw_debug_info(SDL_Renderer *, TTF_Font *);

int make_commands_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int check_command_status(COMMAND_STATUS *, int, int);
int get_command_triangle(int *, int *, int *, int *, int *, int *);

int make_commands_back_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int make_status_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int check_status_status(STATUS_STATUS *, int, int);
int get_status_triangle(int *, int *, int *, int *, int *, int *);

int make_hp_and_mp_window(SDL_Renderer *, TTF_Font *, SDL_Event);
int convert_int_to_full_width_char(int, char *);
int convert_int_to_alphabet(int, char *);

int knock_out_monster(SDL_Renderer *renderer, SDL_Event, char *, int, MONSTER *, char *);
int compare_agility(const void *, const void *);
int create_battle_status_window(SDL_Renderer *);

/*** message list ***/
#define TALK_MESSAGE "そっちには　だれも　いないよ。"
#define SEARCH_MESSAGE "あしもとを　しらべたけど　なにもないよ。"
#define TRESUREBOX_MESSAGE "　をてにいれた！"
#define TRESUREBOX_EMPTY_MESSAGE "からっぽ！"
#define DOOR_MESSAGE "そのほうこうに　とびらは　ないよ!"
#define ENCOUNT_MESSAGE "　があらわれた！"

/*** Initialize value ***/

#define FONT_PATH "font/PixelMplus12-Regular.ttf"


#endif

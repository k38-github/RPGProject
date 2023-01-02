#ifndef PARTS_H
#define PARTS_H

/*
 * macro definition
 */
/*** message list ***/
#define TALK_MESSAGE "そっちには　だれも　いないよ。"
#define SEARCH_MESSAGE "あしもとを　しらべたけど　なにもないよ。"
#define TRESUREBOX_MESSAGE "　をてにいれた！"
#define TRESUREBOX_EMPTY_MESSAGE "からっぽ！"
#define DOOR_MESSAGE "そのほうこうに　とびらは　ないよ!"
#define ENCOUNT_MESSAGE "　があらわれた！"

/*** Initialize value ***/

#define FONT_PATH "src/resources/font/PixelMplus12-Regular.ttf"

/*
 * type definition
 */
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

/*
 * prototype declaration
 */

/*
 * global variable external reference declaration
 */
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int IMAGE_WIDTH;
extern const int IMAGE_HEIGHT;
extern const int MAGNIFICATION;
extern const int GRID_SIZE;
extern const int FONT_SIZE;

extern int GOLD;

extern int ROW;
extern int COL;
extern int OUT_OF_MAP;

extern char MAP_EVENT_NAME[256];
extern int map_array[65536];
extern int number_of_map_image;

extern NPC npc[256];
extern int number_of_npc_image;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *player_image;
extern TTF_Font *font;
extern TTF_Font *title_font;
extern TTF_Font *main_title_font;


extern int animecycle;
extern int speed;
extern int frame;
extern int number_of_monster;

extern CHARACTER player;
extern MONSTER monster[256];

extern TREASURE_FRAME treasure[256];
extern DOOR door[256];

extern WINDOW message_window;
extern WINDOW debug_window;
extern WINDOW command_window;
extern WINDOW status_window;
extern WINDOW player_status_window;
extern WINDOW hp_and_mp_window;
extern WINDOW gold_window;

extern WINDOW battle_back_window;
extern WINDOW battle_status_window;
extern WINDOW battle_select_window;
extern WINDOW battle_action_window;
extern WINDOW battle_enemy_window;

extern char *message;

extern STATE state;
extern STATE debug_state;
extern STATE flash_triangle_status;
extern COMMAND_STATUS command_status;
extern STATUS_STATUS status_status;


#endif

// Sheep Kaboom | BitBitJam 4
// Imanol Barriuso (Imanolea) 2017
#include <gb/gb.h>
#include <gb/hardware.h>
#include <rand.h>
// Data files
// Tiles
#include "data/tiles/sprite_tileset.h"
#include "data/tiles/sprite_bkg_tileset.h"
#include "data/tiles/bkg_tileset.h"
// Maps
#include "data/maps/game_map.h"
#include "data/maps/gui_map.h"

// Constants
// Systems
#define STD_ORIENTATION     0U
#define INV_ORIENTATION     S_FLIPX
#define STD_PALETTE         0U    
#define INV_PALETTE         S_PALETTE
// Flags
#define CHANGEANIM_F        1U
// GUI
#define GUI_SCORE_X         17U
// Tiles
#define DIGIT0_TILE         192U
// States
#define SHEEP_IDLE_ST       0U
#define SHEEP_FIRED_ST      1U // state_t0: movx and -movy, state_t1: gravity
#define SHEEP_PREP_ST       2U // sheep is getting prepared to be fired
// Game
#define MIN_SHOOT_PW        0U
#define MAX_SHOOT_PW        15U
#define SHEEP_CANNON_X      36U
#define SHEEP_CANNON_Y      132U

// Data tables
UBYTE frame_list[] = {
    0, 0x10,   0, 0x10, // sprite_num, sprite_att | 0 - Null frame
    4, 0x10,   6, 0x10, // 4 - Sheep looking right
    8, 0x10,  10, 0x10, // 8 - Sheep looking down
    6, 0x70,   4, 0x70, //12 - Sheep looking left
   10, 0x70,   8, 0x70  //16 - Sheep looking up
};

UWORD frame_table[] = {
    0,  // 0 - Null frame
    4,  // 1 - Sheep looking right
    8,  // 2 - Sheep looking down
   12,  // 3 - Sheep looking left
   16   // 4 - Sheep looking up
};

UBYTE animation_list[] = {
    0, 254, 255,                                // frame_table, animation_t | 0 - Null animation
    1,   8,   2,   8,   3,   8,   4,   8, 255   // 3 - Rolling sheep
};

UWORD animation_table[] = {
    0,  // 0 - Null animation
    3   // 1 - Rolling sheep
};

// Structures
typedef struct {
    UBYTE null;
    UBYTE x;
    UBYTE y;
    UBYTE frame;
    UBYTE palette;
    UBYTE orientation;
    UBYTE animation_no;
    UBYTE animation_f;
    UBYTE animation_i;
    UBYTE animation_t;
    UBYTE state;
    UBYTE state_t0;
    UBYTE state_t1;
} Character;

// Variables
UBYTE cur_joypad;
UBYTE pre_joypad;
UBYTE bkg_x;
UBYTE bkg_y;
Character sheep;
UBYTE sheep_shoot_power;
UBYTE score;
UBYTE sprite_no_i;
UBYTE logic_counter;
// Lists
UBYTE gui_score_tiles[3];

// Functions
void init_game_interrupts();
void init_game_palettes();
void game();
void init_game();
void init_game_var();
void init_game_sprite();
void init_game_bkg();
void init_game_gui();
void logic();
void read_joypad();
void logic_sheep();
void logic_sheep_actions();
void logic_sheep_actions_shoot();
void logic_sheep_state();
void logic_sheep_state_fired();
void logic_sheep_state_prep();
void logic_sheep_state_idle();
void upd();
void upd_sheep();
void upd_characters();
void upd_character(Character *character);
void upd_character_animation(Character *character);
void upd_character_sprite(Character *character);
void upd_gui();
void upd_gui_score();
void draw_gui();
void draw_bkg();

void main() {
    init_game_interrupts();
    init_game_palettes();
    game();
}

void init_game_interrupts() {
    disable_interrupts();
    DISPLAY_OFF;
    add_VBL(draw_gui);
    add_VBL(draw_bkg);
    DISPLAY_ON;
    enable_interrupts();
}

void init_game_palettes() {
    BGP_REG = 0xE4;
    OBP0_REG = 0xE4;
    OBP1_REG = 0x1E;
}

void game() {
    init_game();
    initrand(LY_REG);
    // Main loop
    while (1) {
        logic();
        upd();
        score = sheep_shoot_power;
        wait_vbl_done();
    }
}

void init_game() {
    disable_interrupts();
    DISPLAY_OFF;
    init_game_var();
    init_game_sprite();
    init_game_bkg();
    init_game_gui();
    DISPLAY_ON;
    enable_interrupts();
}

void init_game_var() {
    cur_joypad = 0;
    pre_joypad = 0;
    bkg_x = 0;
    bkg_y = 0;
    // Sheep
    sheep.x = 80;
    sheep.y = 80;
    sheep.frame = 0;
    sheep.palette = STD_PALETTE;
    sheep.orientation = STD_ORIENTATION;
    sheep.animation_no = 1;
    sheep.animation_f = 0;
    sheep.animation_i = 0;
    sheep.animation_t = 1;
    sheep.state = SHEEP_IDLE_ST;
    sheep.state_t0 = 0;
    sheep.state_t1 = 0;
    sheep_shoot_power = MIN_SHOOT_PW;
    score = 0;
    sprite_no_i = 0;
    logic_counter = 0;
    gui_score_tiles[0] = 0;
    gui_score_tiles[1] = 0;
    gui_score_tiles[2] = 0;
}

void init_game_sprite() {
    SWITCH_ROM_MBC1(sprite_tilesetBank);
    set_sprite_data(0, 128, sprite_tileset);
    SPRITES_8x16;
    upd_character_sprite(&sheep);
    SHOW_SPRITES;
}

void init_game_bkg() {
    SWITCH_ROM_MBC1(bkg_tilesetBank);
    set_bkg_data(0, 128, bkg_tileset);
    SWITCH_ROM_MBC1(game_mapBank);
    set_bkg_tiles(0, 0, 32, 32, game_map);
    move_bkg(bkg_x, bkg_y);
    SHOW_BKG;
}

void init_game_gui() {
    SWITCH_ROM_MBC1(sprite_bkg_tilesetBank);
    set_win_data(128, 256, sprite_bkg_tileset);
    SWITCH_ROM_MBC1(gui_mapBank);
    set_win_tiles(0, 0, gui_mapWidth, gui_mapHeight, gui_map);
    move_win(7, 128);
    SHOW_WIN;
}

void logic() {
    read_joypad();
    logic_sheep();
    logic_counter++;
}

void read_joypad() {
    pre_joypad = cur_joypad;
    cur_joypad = joypad();
}

void logic_sheep() {
    logic_sheep_actions();
    logic_sheep_state();
}

void logic_sheep_actions() {
    if (cur_joypad & J_A && !(pre_joypad & J_A)) { // A key down
        if (sheep.state != SHEEP_FIRED_ST) {
            sheep.state = SHEEP_PREP_ST; // If the sheep is not being fired it gets ready to be fired
        }
    }
    if (!(cur_joypad & J_A) && pre_joypad & J_A) { // A key up
        if (sheep.state == SHEEP_PREP_ST) {
            logic_sheep_actions_shoot(); // If the sheep is ready to get fired it gets fired
        }
    }
}

void logic_sheep_actions_shoot() {
    sheep.x = SHEEP_CANNON_X;
    sheep.y = SHEEP_CANNON_Y;
    sheep.state = SHEEP_FIRED_ST;
    sheep.state_t0 = sheep_shoot_power; // initial movx and -movy
    sheep.state_t1 = 0; // gravity acceleration
}

void logic_sheep_state() {
    if (sheep.state == SHEEP_FIRED_ST) {
        logic_sheep_state_fired();
    } else if (sheep.state == SHEEP_PREP_ST) {
        logic_sheep_state_prep();
    } else if (sheep.state == SHEEP_IDLE_ST) {
        logic_sheep_state_idle();
    }
}

void logic_sheep_state_fired() {
    UBYTE movx;
    movx = sheep.state_t0 >> 2;
    if ((sheep.state_t0 & 0x03) > (logic_counter & 0x03)) {
        movx++;
    }
    sheep.x += movx;
    sheep.y += - (movx << 1);
    sheep.y += sheep.state_t1; // gravity acceleration
    if ((logic_counter & 0x03) == 0) {
        sheep.state_t1++;
    }
}

void logic_sheep_state_prep() {
    if (logic_counter & 0x03) {
        return;
    }
    sheep_shoot_power++;
    if (sheep_shoot_power == (MAX_SHOOT_PW)) {
        logic_sheep_actions_shoot();
    }
}

void logic_sheep_state_idle() {
    sheep.x = SHEEP_CANNON_X;
    sheep.y = SHEEP_CANNON_Y;
    sheep_shoot_power = 0;
}

void upd() {
    upd_sheep();
    upd_characters();
    upd_gui();
}

void upd_sheep() {
    if (sheep.x > SCREENWIDTH + 16 || sheep.y > SCREENHEIGHT + 16) {
        sheep.state = SHEEP_IDLE_ST;
    }
}

void upd_characters() {
    sprite_no_i = 0;
    upd_character(&sheep);
}

void upd_character(Character *character) {
    upd_character_animation(character);
    upd_character_sprite(character);
}

void upd_character_animation(Character *character) {
    UBYTE *animation_info;
    UBYTE *animation_start;
    if (character->animation_t-- != 0 && character->animation_f != CHANGEANIM_F) {
        return;
    }
    animation_info = &animation_list;
    animation_info += animation_table[character->animation_no];
    animation_start = animation_info;
    animation_info += character->animation_i;
    if (*animation_info == 255) {
        character->animation_i = 0;
        animation_info = animation_start;
    }
    character->frame = *animation_info++;
    character->animation_t = *animation_info;
    character->animation_i += 2;
    character->animation_f = 0;
}

void upd_character_sprite(Character *character) {
    UBYTE *frame_info;
    frame_info = &frame_list;
    frame_info += frame_table[character->frame];
    set_sprite_tile(sprite_no_i, *frame_info++);
    set_sprite_prop(sprite_no_i, *frame_info++ ^ character->palette);
    move_sprite(sprite_no_i, character->x, character->y);
    sprite_no_i++;
    set_sprite_tile(sprite_no_i, *frame_info++);
    set_sprite_prop(sprite_no_i, *frame_info++ ^ character->palette);
    move_sprite(sprite_no_i, character->x + 8, character->y);
}

void upd_gui() {
    upd_gui_score();
}

void upd_gui_score() {
    UBYTE div_rem;
    UBYTE score_aux;
    UBYTE pre_score_aux;
    BYTE i;
    score_aux = score;
    for (i = 2; i != -1; i--) {
        pre_score_aux = score_aux;
        score_aux = score_aux / 10;
        div_rem = pre_score_aux - (score_aux * 10);
        gui_score_tiles[i] = DIGIT0_TILE + div_rem;      
    }
}

void draw_gui() {
    set_win_tiles(GUI_SCORE_X, 0, 3, 1, gui_score_tiles);
}

void draw_bkg() {
    move_bkg(bkg_x, bkg_y);
}
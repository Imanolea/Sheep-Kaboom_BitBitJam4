// Sheep Kaboom | BitBitJam 4
// Light Games 2017
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
#define NOANIM_F            2U
#define FREEPOS_F           0U
#define OCCUPIEDPOS_F       1U
// GUI
#define GUI_SCORE_X         14U
#define GUI_SCORE_Y         1U
#define GUI_POWER_X         5U
#define GUI_POWER_Y         0U
#define GUI_TIME_X          5U
#define GUI_TIME_Y          1U
// Tiles
#define DIGIT0_TILE         192U
#define ST_POWERFULL_TILE   252U
#define ST_NOPOWER_TILE     254U
#define ED_POWERFULL_TILE   253U
#define ED_NOPOWER_TILE     255U
#define MD_POWERFULL_TILE   250U
#define MD_NOPOWER_TILE     251U

// States
#define SHEEP_IDLE_ST       0U
#define SHEEP_FIRED_ST      1U // state_t0: movx and -movy, state_t1: gravity
#define SHEEP_PREP_ST       2U // sheep is getting prepared to be fired
#define TARGET_DISABLED_ST  0U
#define TARGET_BAD_ST       1U
#define TARGET_GOOD_ST      2U
// Game
#define MIN_SHOOT_PW        3U
#define MAX_SHOOT_PW        18U
#define SHEEP_CANNON_X      38U
#define SHEEP_CANNON_Y      121U
#define SHEEP_COLL_AREA     10U
#define START_TIME          99U
#define TARGET_NUM          7U
#define TARGET_POS_NUM      16U

// Data tables
UBYTE frame_list[] = {
    0, 0x10,   0, 0x10, // sprite_num, sprite_att | 0 - Null frame
    4, 0x10,   6, 0x10, // 4 - Sheep looking right
    8, 0x10,  10, 0x10, // 8 - Sheep looking down
    6, 0x70,   4, 0x70, //12 - Sheep looking left
   10, 0x70,   8, 0x70, //16 - Sheep looking up
   12, 0x00,  14, 0x00, //20 - Cactus
   16, 0x00,  18, 0x00  //24 - Velcro target
};

UWORD frame_table[] = {
    0,  // 0 - Null frame
    4,  // 1 - Sheep looking right
    8,  // 2 - Sheep looking down
   12,  // 3 - Sheep looking left
   16,  // 4 - Sheep looking up
   20,  // 5 - Cactus
   24   // 6 - Velcro target
};

UBYTE animation_list[] = {
    0, 254, 255,                                // frame_table, animation_t | 0 - Null animation
    1,   8,   2,   8,   3,   8,   4,   8, 255,  // 3 - Rolling sheep
    1, 254, 255,                                //12 - Idle sheep
    5, 254, 255,                                //15 - Idle cactus
    6, 254, 255                                 //18 - Idle velcro target
};

UWORD animation_table[] = {
    0,  // 0 - Null animation
    3,  // 1 - Rolling sheep
   12,  // 2 - Idle sheep
   15,  // 3 - Idle cactus
   18   // 4 - Idle velcro target
};

UBYTE target_positions[] = {
     FREEPOS_F,  80, 48,   0,    // occupation_f, posx, posy, (noused)
     FREEPOS_F,  80, 80,   0,
     FREEPOS_F,  80,112,   0,
     FREEPOS_F, 112, 48,   0,
     FREEPOS_F, 112, 80,   0,
     FREEPOS_F, 112,112,   0,
     FREEPOS_F, 144, 48,   0,
     FREEPOS_F, 144, 80,   0,
     FREEPOS_F, 144,112,   0,
     FREEPOS_F,  96, 32,   0,
     FREEPOS_F,  96, 64,   0,
     FREEPOS_F,  96, 96,   0,
     FREEPOS_F, 128, 32,   0,
     FREEPOS_F, 128, 64,   0,
     FREEPOS_F, 128, 96,   0,
     FREEPOS_F, 144, 16,   0
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
UBYTE sheep_shoot_power_inc;
Character targets[TARGET_NUM];
UBYTE target_spawn_t;
UWORD score;
UBYTE remaining_time;
UBYTE remaining_time_t;
UBYTE sprite_no_i;
UBYTE logic_counter;
// Lists
UBYTE gui_score_tiles[4];
UBYTE gui_time_tiles[2];
UBYTE gui_power_tiles[15];

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
void logic_game();
void logic_game_spawn_target();
void try_spawn_target(Character *character);
void spawn_target(Character *character, UBYTE *target_positions_pointer, UBYTE target_positions_i);
void logic_sheep();
void logic_sheep_actions();
void logic_sheep_actions_shoot();
void logic_sheep_state();
void logic_sheep_state_fired();
void logic_sheep_state_prep();
void logic_sheep_state_idle();
void logic_sheep_collision();
void logic_targets();
void logic_target(Character *character);
void sheep_collision(Character *character);
void upd();
void upd_time();
void upd_sheep();
void upd_characters();
void upd_character(Character *character);
void upd_character_animation(Character *character);
void upd_character_sprite(Character *character);
void upd_gui();
void upd_gui_score();
void upd_gui_time();
void upd_gui_power();
void draw_gui();
void draw_bkg();
void change_character_animation(Character *character, UBYTE animation_no);

// Ensamblador
WORD div8(UBYTE numerator, UBYTE denominator);
WORD mul8(UBYTE factor_1, UBYTE factor_2);
WORD div16(UWORD numerator, UBYTE denominator, UWORD result_address);
WORD mul16(UWORD factor_1, UBYTE factor_2);

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
    initrand(LY_REG);
    init_game();
    // Main loop
    while (1) {
        logic();
        upd();
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
    UBYTE i;
    Character *targets_pointer;
    UBYTE *gui_score_tiles_pointer;
    UBYTE *gui_time_tiles_pointer;
    UBYTE *gui_power_tiles_pointer;
    cur_joypad = 0;
    pre_joypad = 0;
    bkg_x = 0;
    bkg_y = 0;
    // Sheep
    sheep.x = SHEEP_CANNON_X;
    sheep.y = SHEEP_CANNON_Y;
    sheep.frame = 0;
    sheep.palette = STD_PALETTE;
    sheep.orientation = STD_ORIENTATION;
    sheep.animation_no = 2;
    sheep.animation_f = 0;
    sheep.animation_i = 0;
    sheep.animation_t = 1;
    sheep.state = SHEEP_IDLE_ST;
    sheep.state_t0 = 0;
    sheep.state_t1 = 0;
    sheep_shoot_power = MIN_SHOOT_PW;
    sheep_shoot_power_inc = 1;
    targets_pointer = &targets;
    for (i = 0; i != TARGET_NUM; i++) {
        targets_pointer->x = 0;
        targets_pointer->y = 0;
        targets_pointer->frame = 0;
        targets_pointer->palette = STD_PALETTE;
        targets_pointer->orientation = STD_ORIENTATION;
        targets_pointer->animation_no = 0;
        targets_pointer->animation_f = NOANIM_F;
        targets_pointer->animation_i = 0;
        targets_pointer->animation_t = 1;
        targets_pointer->state = TARGET_DISABLED_ST;
        targets_pointer->state_t0 = 0;
        targets_pointer->state_t1 = 0;
        targets_pointer++;
    }
    target_spawn_t = 0;
    score = 0;
    remaining_time = START_TIME;
    sprite_no_i = 0;
    logic_counter = 0;
    gui_score_tiles_pointer = &gui_score_tiles;
    for (i = 0; i != 4; i++) {
        *gui_score_tiles_pointer++ = 0;
    }
    gui_time_tiles_pointer = &gui_time_tiles;
    *gui_time_tiles_pointer++ = DIGIT0_TILE + 9;
    *gui_time_tiles_pointer = DIGIT0_TILE + 9;
    gui_power_tiles_pointer = &gui_power_tiles;
    *gui_power_tiles_pointer++ = ST_NOPOWER_TILE;
    for (i = 1; i != 14; i++) {
        *gui_power_tiles_pointer++ = MD_NOPOWER_TILE;
    }
    *gui_power_tiles_pointer = ED_NOPOWER_TILE;
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
    logic_game();
    logic_sheep();
    logic_targets();
    logic_counter++;
}

void read_joypad() {
    pre_joypad = cur_joypad;
    cur_joypad = joypad();
}

void logic_game() {
    logic_game_spawn_target();
}

void logic_game_spawn_target() {
    UBYTE i;
    Character *targets_pointer;
    if (target_spawn_t++ != 255) {
        return;
    }
    target_spawn_t = 0;
    targets_pointer = &targets;
    for (i = 0; i < TARGET_NUM; i++) {
        if (targets_pointer->state == TARGET_DISABLED_ST) {
            try_spawn_target(targets_pointer);
            return;
        }
        targets_pointer++;
    }
}

void try_spawn_target(Character *character) {
    UBYTE *target_positions_pointer;
    UBYTE target_positions_i;
    UBYTE i;
    target_positions_i = rand() & 0x0F;
    target_positions_pointer = &target_positions;
    target_positions_pointer += target_positions_i << 2;
    for (i = 0; i != TARGET_POS_NUM; i++) {
        target_positions_i++;
        target_positions_pointer += 4;
        if (target_positions_i == TARGET_POS_NUM) {
            target_positions_i = 0;
            target_positions_pointer = &target_positions;
        }
        if (*target_positions_pointer == FREEPOS_F) {
            spawn_target(character, target_positions_pointer, target_positions_i);
            return;
        }
    }
}

void spawn_target(Character *character, UBYTE *target_positions_pointer, UBYTE target_positions_i) {
    *target_positions_pointer++ = OCCUPIEDPOS_F;
    character->x = *target_positions_pointer++;
    character->y = *target_positions_pointer;
    character->state_t0 = target_positions_i;
    if (rand() & 0x07) {
        character->frame = 6;
        character->animation_no = 4;
        character->state = TARGET_GOOD_ST;
    } else {
        character->frame = 5;
        character->animation_no = 3;
        character->state = TARGET_BAD_ST;
    }
}

void logic_sheep() {
    logic_sheep_actions();
    logic_sheep_state();
    logic_sheep_collision();
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
    change_character_animation(&sheep, 1);
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
    if (logic_counter & 0x01) {
        return;
    }
    if (sheep_shoot_power == MAX_SHOOT_PW) {
        sheep_shoot_power_inc = -1;
    }
    sheep_shoot_power += sheep_shoot_power_inc;
    if (sheep_shoot_power == 0) {
        logic_sheep_actions_shoot();
    }
}

void logic_sheep_state_idle() {
    sheep.x = SHEEP_CANNON_X;
    sheep.y = SHEEP_CANNON_Y;
    sheep_shoot_power = MIN_SHOOT_PW;
    sheep_shoot_power_inc = 1;
}

void logic_sheep_collision() {
    UBYTE left_lim, right_lim, upper_lim, bottom_lim;
    UBYTE target_x;
    UBYTE target_y;
    UBYTE i;
    Character *targets_pointer;
    left_lim = sheep.x - SHEEP_COLL_AREA;
    right_lim = sheep.x + SHEEP_COLL_AREA;
    upper_lim = sheep.y - SHEEP_COLL_AREA;
    bottom_lim = sheep.y + SHEEP_COLL_AREA;
    targets_pointer = &targets;
    for (i = 0; i < TARGET_NUM; i++) {
        target_x = targets_pointer->x;
        target_y = targets_pointer->y;
        if (left_lim < target_x &&
        right_lim > target_x &&
        upper_lim < target_y &&
        bottom_lim > target_y) {
            sheep_collision(targets_pointer);
        }
        targets_pointer++;
    }
}

void sheep_collision(Character *character) {
    UBYTE *target_positions_pointer; 
    if (character->state == TARGET_DISABLED_ST) {
        return;
    } else if (character->state == TARGET_BAD_ST) {
        remaining_time -= 10;
        remaining_time_t = 0;
    } else if (character->state == TARGET_GOOD_ST) {
        score++;
    }
    sheep.state = SHEEP_IDLE_ST;
    change_character_animation(&sheep, 2);
    character->frame = 0;
    character->state = TARGET_DISABLED_ST;
    target_positions_pointer = &target_positions;
    target_positions_pointer += character->state_t0 << 2;
    *target_positions_pointer = FREEPOS_F; // We set free the collided target
}

void logic_targets() {
    UBYTE i;
    Character *targets_pointer;
    targets_pointer = &targets;
    for (i = 0; i < TARGET_NUM; i++) {
        logic_target(targets_pointer++);
    }
}

void logic_target(Character *character) {
    if (character->state == TARGET_DISABLED_ST) {
        return;
    }
}

void upd() {
    upd_time();
    upd_sheep();
    upd_characters();
    upd_gui();
}

void upd_time() {
    if (remaining_time_t++ != 60) {
        return;
    }
    remaining_time_t = 0;
    remaining_time--;
}

void upd_sheep() {
    if (sheep.x > SCREENWIDTH + 16 || sheep.y > SCREENHEIGHT + 16) {
        sheep.state = SHEEP_IDLE_ST;
        change_character_animation(&sheep, 2);
    }
}

void upd_characters() {
    UBYTE i;
    Character *targets_pointer;
    sprite_no_i = 0;
    upd_character(&sheep);
    targets_pointer = &targets;
    for (i = 0; i != TARGET_NUM; i++) {
        upd_character(targets_pointer++);
    }
}

void upd_character(Character *character) {
    upd_character_animation(character);
    upd_character_sprite(character);
}

void upd_character_animation(Character *character) {
    UBYTE *animation_info;
    UBYTE *animation_start;
    UWORD *animation_table_pointer;
    if (character->animation_f == NOANIM_F) {
        return;
    }
    if (character->animation_t-- != 0 && character->animation_f != CHANGEANIM_F) {
        return;
    }
    animation_info = &animation_list;
    animation_table_pointer = &animation_table;
    animation_table_pointer += character->animation_no;
    animation_info += *animation_table_pointer;
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
    UWORD *frame_table_pointer;
    frame_info = &frame_list;
    frame_table_pointer = &frame_table;
    frame_table_pointer += character->frame;
    frame_info += *frame_table_pointer;
    set_sprite_tile(sprite_no_i, *frame_info++);
    set_sprite_prop(sprite_no_i, *frame_info++ ^ character->palette);
    move_sprite(sprite_no_i, character->x, character->y);
    sprite_no_i++;
    set_sprite_tile(sprite_no_i, *frame_info++);
    set_sprite_prop(sprite_no_i, *frame_info++ ^ character->palette);
    move_sprite(sprite_no_i, character->x + 8, character->y);
    sprite_no_i++;
}

void upd_gui() {
    upd_gui_score();
    upd_gui_time();
    upd_gui_power();
}

void upd_gui_score() {
    UBYTE div_rem;
    UWORD score_aux;
    UBYTE *gui_score_tiles_pointer;
    BYTE i;
    score_aux = score;
    gui_score_tiles_pointer = &gui_score_tiles;
    gui_score_tiles_pointer += 3;
    for (i = 3; i != -1; i--) {
        div_rem = div16(score_aux, 10, &score_aux);
        *gui_score_tiles_pointer-- = DIGIT0_TILE + div_rem;    
    }
}

void upd_gui_time() {
    UWORD div_out;
    UBYTE div_rem;
    UBYTE time_aux;
    UBYTE *gui_time_tiles_pointer;
    BYTE i;
    time_aux = remaining_time;
    gui_time_tiles_pointer = &gui_time_tiles;
    gui_time_tiles_pointer += 1;
    for (i = 1; i != -1; i--) {
        div_out = div8(time_aux, 10);
        time_aux = div_out;
        div_rem = div_out >> 8;
        *gui_time_tiles_pointer-- = DIGIT0_TILE + div_rem;   
    }
}

void upd_gui_power() {
    BYTE sheep_shoot_power_aux;
    UBYTE *gui_power_tiles_pointer;
    UBYTE i;
    gui_power_tiles_pointer = &gui_power_tiles;
    sheep_shoot_power_aux = sheep_shoot_power;
    sheep_shoot_power_aux -= MIN_SHOOT_PW;
    sheep_shoot_power_aux--;
    if (sheep_shoot_power_aux < 0) {
        *gui_power_tiles_pointer++ = ST_NOPOWER_TILE;
    } else {
        *gui_power_tiles_pointer++ = ST_POWERFULL_TILE;
    }
    for (i = 1; i != 14; i++) {
        sheep_shoot_power_aux--;
        if (sheep_shoot_power_aux < 0) {
            *gui_power_tiles_pointer++ = MD_NOPOWER_TILE;
        } else {
            *gui_power_tiles_pointer++ = MD_POWERFULL_TILE;
        }
    }
    sheep_shoot_power_aux--;
    if (sheep_shoot_power_aux < 0) {
        *gui_power_tiles_pointer = ED_NOPOWER_TILE;
    } else {
        *gui_power_tiles_pointer = ED_POWERFULL_TILE;
    }
}

void draw_gui() {
    set_win_tiles(GUI_SCORE_X, GUI_SCORE_Y, 4, 1, gui_score_tiles);
    set_win_tiles(GUI_TIME_X, GUI_TIME_Y, 2, 1, gui_time_tiles);
    set_win_tiles(GUI_POWER_X, GUI_POWER_Y, 15, 1, gui_power_tiles);
}

void draw_bkg() {
    move_bkg(bkg_x, bkg_y);
}

void change_character_animation(Character *character, UBYTE animation_no) {
    character->animation_no = animation_no;
    character->animation_f = CHANGEANIM_F;
    character->animation_i = 0;
    character->animation_t = 1;
}
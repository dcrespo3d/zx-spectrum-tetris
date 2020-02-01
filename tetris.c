////////////////////////////////////////////////////////////////////////////////
//
// Tetris for ZX Spectrum 48K
//
// (C) 2019, 2020 David Crespo - https://github.com/dcrespo3d/zx-spectrum-tetris
//
////////////////////////////////////////////////////////////////////////////////
#include "params0.h"
#include "screen.h"
#include "keyb.h"
#include "colors.h"
#include "frame.h"
#include "menu.h"
#include "tile.h"
#include "text.h"
#include "blocks.h"

#include <stdio.h>
#include <stdlib.h>

// color definitions
#define COLOR_BG         ZX_BLK_WHI
#define COLOR_FRAME      ZX_BLK_BLU
#define COLOR_FRAME_OVER ZX_BLK_RED
#define PAPER_MSG        ZX_BLK
#define INK_MSG          ZX_CYA
#define COLOR_MSG_OVER   ZX_BLK_YEL
#define PAPER_OVER       ZX_YEL
#define INK_OVER         ZX_BLK

// assembler inline shortcut for HALT
#define HALT {asm("HALT");}

// directions
#define DIRN 0x00   // no direction
#define DIRB 0x08   // bottom
#define DIRL 0x04   // left
#define DIRR 0x02   // right
#define DIRT 0x01   // top

// board limits
char min_x;
char min_y;
char max_x;
char max_y;

// current and previous state of block
char curr_x;
char curr_y;
char curr_r;
char curr_b;

char prev_x;
char prev_y;
char prev_r;
char prev_b;

char next_x;
char next_y;
char next_r;
char next_b;

char future_b;

// game running flag, 1 for game in progress, 0 for game over
char game_running;

// pause flag, 1 for paused.
char paused = 0;
// pause lock prevents continous 0 -> 1 -> 0 -> 1 cycle when P key pressed.
char pauselock = 0;

// previous, current and next directions (values are DIR*)
char prev_dir;
char curr_dir;
char next_dir;
char next_inc;

// score points
uword score_counter;

// block colors
ubyte block_colors[7] = {
    ZX_BLK_YEL,
    ZX_BLK_MAG,
    ZX_BLK_GRE,
    ZX_BLK_CYA,
    ZX_BLK_WHI,
    ZX_BLK_RED,
    ZX_BLK_BLU
};

#define BCOLS 10
#define BROWS 20

char line00[BCOLS];
char line01[BCOLS];
char line02[BCOLS];
char line03[BCOLS];
char line04[BCOLS];
char line05[BCOLS];
char line06[BCOLS];
char line07[BCOLS];
char line08[BCOLS];
char line09[BCOLS];
char line10[BCOLS];
char line11[BCOLS];
char line12[BCOLS];
char line13[BCOLS];
char line14[BCOLS];
char line15[BCOLS];
char line16[BCOLS];
char line17[BCOLS];
char line18[BCOLS];
char line19[BCOLS];

char *block_lines[BROWS] = {
    line00, line01, line02, line03, line04, line05, line06, line07, line08, line09,
    line10, line11, line12, line13, line14, line15, line16, line17, line18, line19,
};

// speed is controlled forcing a move down only each N frames.

// this variable takes value from skill_halt_table
char halts_per_move_down = 50;
char halts_until_move_down;


void reset_block_lines()
{
    for (char r = 0; r < BROWS; r++) {
        char* line = block_lines[r];
        for (char c = 0; c < BCOLS; c++) {
            line[c] = 0;
        }
    }
}

// table of halts for skill level
//                             0   1   2   3   4   5   6   7   8   9
char skill_halt_table[10] = { 80, 65, 50, 40, 30, 24, 16, 10, 7, 5 };

// top screen message
//                         1         2         3         4         5         6
//               0123456789012345678901234567890123456789012345678901234567890123
char* top_message = "Level X  |  WASD to move  |   P to pause   |  Score :";

// game over message
//                           1         2         3         4         5         6
//                 0123456789012345678901234567890123456789012345678901234567890123
char* gover_msg =    "  GAME OVER  |  press ENTER to play again  |  M for menu  ";

// some forward declarations
void restart_game();
void print_top_message();
void print_score();
void draw_future_block();

// initialize stdlib random number generator using the R register
void init_rand()
{
    __asm
        LD   A, R
        LD  (_param0b), A
    __endasm
    srand(param0b);
    rand();
}

// clear the screen with default colors
void clear_screen()
{
    screen_border_set(ZX_BLK);
    screen_clear_bmap(0x00);
    screen_clear_attr(COLOR_BG);
}

// draw game over traits
void do_game_over()
{
    game_running = 0;
    draw_frame(0, 32, 1, 23, COLOR_FRAME_OVER);
    screen_clear_attr_blocks(COLOR_MSG_OVER, 0, 0, 32, 1);
    text_print(gover_msg, 3, 12, PAPER_OVER, INK_OVER);
}

// reset variables to initial state
void reset_vars()
{
    min_x = 11;
    max_x = 20;
    min_y = 3;
    max_y = 22;

    curr_x = 14;
    curr_y = 3;
    curr_b = 0;
    curr_r = 0;

    prev_x = 14;
    prev_y = 3;
    prev_b = -1;
    prev_r = 0;

    prev_dir = DIRN;
    curr_dir = DIRR;
    next_dir = DIRR;

    halts_until_move_down = halts_per_move_down;

    score_counter = 0;
    game_running = 1;
}

void clear_block(char blk, char rot, char xb, char yb)
{
    if (blk < 0) return;

    char* block = (&TBLK_J) + (64*blk) + (16*rot);

    char* bptr = block;
/*
    for (char y = 0; y < 4; y++) {
        param0w = screen_get_bmap_addr(xb, y+yb);
        for (char x = 0; x < 4; x++) {
            param0b = *bptr++;
            if (param0b)
                param_clear_tile();
            param0w++;
        }
    }

    bptr = block;
*/
    for (char y = 0; y < 4; y++) {
        param0w = screen_get_attr_addr(xb, y+yb);
        for (char x = 0; x < 4; x++) {
            param0b = *bptr++;
            if (param0b)
                *((char*)param0w) = ZX_BLK_BLK;
            param0w++;
        }
    }
}

void draw_block(char blk, char rot, char xb, char yb)
{
    char* bptr = &TBLK_J + (64*blk) + (16*rot);

    char* attr;

    char col = block_colors[blk];

    for (char y = 0; y < 4; y++) {
        attr = (char*)screen_get_attr_addr(xb, y+yb);
        param0w = screen_get_bmap_addr(xb, y+yb);
        for (char x = 0; x < 4; x++) {
            param0b = *bptr++;
            if (param0b) {
                param_draw_tile();
                *attr = col;
            }
            param0w++;
            attr++;
        }
    }

}

char check_valid_block_state(char bb, char br, char bx, char by)
{
    char* bptr = &TBLK_J + (64*bb) + (16*br);

    for (char y = 0; y < 4; y++) {
        char* line = block_lines[by + y - min_y] + (bx - min_x);
        for (char x = 0; x < 4; x++) {
            param0b = *bptr++;
            if (param0b) {
                if (x+bx < min_x) return 0;
                if (x+bx > max_x) return 0;
                if (y+by > max_y) return 0;
                if (line[x] != 0) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

void clear_canvas()
{
    screen_clear_bmap_blocks(0x00    , min_x, min_y, BCOLS, BROWS);
    screen_clear_attr_blocks(COLOR_BG, min_x, min_y, BCOLS, BROWS);
}

void copy_consolidated_to_canvas()
{
    for (char r = 0; r < BROWS; r++)
    {
        char* line = block_lines[r];
        for (char c = 0; c < BCOLS; c++)
        {
            char val = *line++;
            if (val)
            {
                char blk = (val >> 4) - 1;
                char color = block_colors[blk];
                val &= 0x0F;
                param0w = screen_get_bmap_addr(min_x+c, min_y+r);
                param0b = val;
                param_draw_tile();
                screen_print_attr(min_x+c, min_y+r, color);
            }
        }
    }
}

void consolidate(char bb, char br, char bx, char by)
{
    char* bptr = &TBLK_J + (64*bb) + (16*br);

    for (char y = 0; y < 4; y++) {
        char* line = block_lines[by + y - min_y] + (bx - min_x);
        for (char x = 0; x < 4; x++) {
            param0b = *bptr++;
            if (param0b) {
                char val = param0b;
                char blk = (1 + bb) << 4;
                val = val | blk;
                line[x] = val;
            }
        }
    }
}

char block_rows_full_count;
char block_rows_full[4];

void check_rows_full()
{
    char brfidx = 0;
    block_rows_full[0] = 0;
    block_rows_full[1] = 0;
    block_rows_full[2] = 0;
    block_rows_full[3] = 0;

    for (char r = 0; r < BROWS; r++)
    {
        char row_full = 1;
        char* line = block_lines[r];
        for (char c = 0; c < BCOLS; c++)
        {
            if (*line++ == 0)
            {
                row_full = 0;
                break;
            }
        }
        if (row_full) {
            block_rows_full[brfidx++] = r;
        }
    }
    
    block_rows_full_count = brfidx;
}

void remove_full_line(char index)
{
    char* target_line;

    // trim bottom pixels in prev line (above)
    char prev_idx = index - 1;
    if (prev_idx >= 0) {
        target_line = block_lines[prev_idx];
        for (char i = 0; i < BCOLS; i++)
            target_line[i] &= 0xF7;
    }

    // trim top pixels in next line (below)
    char next_idx = index + 1;
    if (next_idx < BROWS) {
        target_line = block_lines[next_idx];
        for (char i = 0; i < BCOLS; i++)
            target_line[i] &= 0xFE;
    }

    target_line = block_lines[index];
    for (char i = 0; i < BCOLS; i++)
        target_line[i] = 0;

    for (char j = index; j >= 1; j--)
        block_lines[j] = block_lines[j-1];

    block_lines[0] = target_line;
}

void remove_full_lines()
{
    if (0 == block_rows_full_count)
        return;

    short delta_score = 0;
    for (char i = 0; i < block_rows_full_count; i++)
    {
        remove_full_line(block_rows_full[i]);
        delta_score += 10;
        score_counter += delta_score;
    }

    clear_canvas();
    copy_consolidated_to_canvas();

    print_score();
}

void rand_future_block()
{
//#define USE_DETERMINISTIC_INDEX
#ifdef  USE_DETERMINISTIC_INDEX
    future_b = (nb++) % 7;
#else
    future_b = rand() % 7;
#endif 
}

void spawn_new_block()
{
    static char nb = 0;

    curr_x = 14;
    curr_y = 2;
    curr_b = future_b;
    curr_r = 0;

    prev_x = 14;
    prev_y = 2;
    prev_b = -1;
    prev_r = 0;

    if (!check_valid_block_state(curr_b, curr_r, curr_x, curr_y))
    {
        do_game_over();
    }
    else
    {
        rand_future_block();
        draw_future_block();
    }
}

void draw_future_block()
{
    char x = 3;
    char y = 5;
    screen_clear_bmap_blocks(0, x, y, 4, 4);
    screen_clear_attr_blocks(ZX_BLK_WHI, x, y-2, 4, 6);
    screen_print_string(x, y-1, "NEXT");
    draw_block(future_b, 0, x, y);
}

// main frame processing function
void update_game()
{
    char saved_dir = -1;
    halts_until_move_down--;
    if (halts_until_move_down == 0) {
        saved_dir = next_dir;
        next_dir = DIRB;
    }

    if (next_dir == DIRN)
    {
        if (prev_b < 0)
        {
            draw_block (curr_b, curr_r, curr_x, curr_y);
            prev_x = curr_x;
            prev_y = curr_y;
            prev_r = curr_r;
            prev_b = curr_b;
        }
        return;
    }

    next_x = curr_x;
    next_y = curr_y;
    next_r = curr_r;
    next_b = curr_b;

    char valid_block = 0;

    // calculate next position from next direction,
    // checking for collisions.
    switch(next_dir) {
        case DIRL:
            next_x = curr_x - 1;
            valid_block = check_valid_block_state(next_b, next_r, next_x, next_y);
            break;
        case DIRR:
            next_x = curr_x + 1;
            valid_block = check_valid_block_state(next_b, next_r, next_x, next_y);
            break;
        case DIRT:
            next_r = (curr_r + 1) & 3;
            valid_block = check_valid_block_state(next_b, next_r, next_x, next_y);
            if (!valid_block) {
                next_x = next_x + 1;
                valid_block = check_valid_block_state(next_b, next_r, next_x+1, next_y);
                if (!valid_block)
                {
                    next_x = next_x - 2;
                    valid_block = check_valid_block_state(next_b, next_r, next_x-1, next_y);
                }
            }
            break;
        case DIRB:
            next_y = curr_y + 1;
            valid_block = check_valid_block_state(next_b, next_r, next_x, next_y);
            halts_until_move_down = halts_per_move_down;
            break;
    }

    if (valid_block)
    {
        curr_x = next_x;
        curr_y = next_y;
        //curr_b = next_b;
        curr_r = next_r;

        clear_block(prev_b, prev_r, prev_x, prev_y);
        draw_block (curr_b, curr_r, curr_x, curr_y);

        prev_x = curr_x;
        prev_y = curr_y;
        prev_r = curr_r;
        prev_b = curr_b;
    }
    else
    {
        if (DIRB == next_dir)
        {
            consolidate(curr_b, curr_r, curr_x, curr_y);
            check_rows_full();
            remove_full_lines();
            spawn_new_block();
        }
    }

    if (-1 == saved_dir)
        next_dir = saved_dir;
}

char Wlock = 0;
char Alock = 0;
char Slock = 0;
char Dlock = 0;

// process keypresses
void process_keyb()
{
    if (game_running)
    {
        next_dir = DIRN;
        if (keyWdown()) { if (!Wlock) { Wlock = 1; next_dir = DIRT; return; }} else Wlock = 0;
        if (keyAdown()) { if (!Alock) { Alock = 1; next_dir = DIRL; return; }} else Alock = 0;
        if (keySdown()) { if (!Slock) { Slock = 1; next_dir = DIRB; return; }} else Slock = 0;
        if (keyDdown()) { if (!Dlock) { Dlock = 1; next_dir = DIRR; return; }} else Dlock = 0;

        // pause key with pauselock for avoiding cycling on -> off -> on -> off
        if (keyPdown()) {
            if (!pauselock) {
                paused = !paused;
                print_top_message();
                pauselock = 1;
                return;
            }
        }
        else {
            pauselock = 0;
        }
    }
    else
    {
        // game over: restart game when ENTER pressed
        if (keyENTdown()) { restart_game(); return; }
    }
}

///////////////////////////////////////////////////////////////////////////////

// main game loop
void enter_game_loop()
{
    // setup game variables initialy
    restart_game();

    while(1)
    {
        // read keypresses and process them
        keyb_read();
        process_keyb();

        // wait for vertical retrace - simple timing
        HALT;

        if (!paused)
            update_game();

        // exit to menu when M pressed
        if (!game_running && keyMdown())
            break;
    }
}

// top message, with variation when paused
void print_top_message()
{
    text_print(top_message, 3, 0, PAPER_MSG, INK_MSG);

    if (paused)
        text_print("P to unpause", 32, 0, PAPER_MSG, INK_MSG);
}

// score on top message
void print_score()
{
    char buf[8];
    sprintf(buf, "%04u", score_counter);
    text_print(buf, 57, 0, PAPER_MSG, INK_MSG);
}

// game initialization / reinitialization
void restart_game()
{
    clear_screen();
    reset_vars();

    draw_frame(min_x-1, 3+max_x-min_x, min_y-1, 3+max_y-min_y, COLOR_FRAME);
    print_top_message();

    print_score();

    reset_block_lines();

    clear_canvas();

//#define TEST_SITUATION
#ifdef  TEST_SITUATION
    for (char r = 16; r <= 19; r++) {
        char* line = block_lines[r];
        for (char c = 0; c <= 8; c++) {
            line[c] = 0x30;
        }
    }

    copy_consolidated_to_canvas();  /// DEBUG DEBUG
#endif

    rand_future_block();

    spawn_new_block();
}

// main entry point
void main()
{
    init_rand();

    while(1)
    {
        // do menu loop until menu exits
        enter_menu_loop();

        
        // after menu exits, set skill level in game
        halts_per_move_down = skill_halt_table[skill_level];
        top_message[6] = '0' + skill_level;

        // do game loop until game exits
        enter_game_loop();
    }

}

// this is not the best practice possible, but it is OK for a small project like this.
// i'm not using a makefile, just a simple "snake_compile" command,
// so i just #include all C sources here.
#include "tile.c"
#include "params0.c"
#include "screen.c"
#include "keyb.c"
#include "frame.c"
#include "menu.c"
#include "text.c"
#include "blocks.c"


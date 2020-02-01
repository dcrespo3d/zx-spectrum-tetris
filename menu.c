////////////////////////////////////////////////////////////////////////////////
//
// Tetris for ZX Spectrum 48K
//
// (C) 2019, 2020 David Crespo - https://github.com/dcrespo3d/zx-spectrum-tetris
//
////////////////////////////////////////////////////////////////////////////////
#include "menu.h"
#include "screen.h"
#include "params0.h"
#include "keyb.h"
#include "tile.h"
#include "text.h"

#define HALT {asm("HALT");}

// tetris
ubyte line1[32] = {0x0,0x4,0x6,0xE,0x2,0x0,0xC,0x6,0x6,0x2,0x0,0x4,0x6,0xE,0x2,0x0,0x0,0xC,0x6,0xA,0x0,0x4,0x6,0xE,0x2,0x0,0xC,0x6,0x6,0x2,0x0,0x0};
ubyte line2[32] = {0x0,0x0,0x0,0x9,0x0,0x0,0x9,0x0,0x0,0x0,0x0,0x0,0x0,0x9,0x0,0x0,0x0,0x9,0x0,0x9,0x0,0x0,0x0,0x9,0x0,0x0,0x9,0x0,0x0,0x0,0x0,0x0};
ubyte line3[32] = {0x0,0x0,0x0,0x9,0x0,0x0,0xD,0x6,0x2,0x0,0x0,0x0,0x0,0x9,0x0,0x0,0xC,0x7,0x6,0xB,0x0,0x0,0x0,0x9,0x0,0x0,0x5,0x6,0x6,0xA,0x0,0x0};
ubyte line4[32] = {0x0,0x0,0x0,0x9,0x0,0x0,0x9,0x0,0x0,0x0,0x0,0x0,0x0,0x9,0x0,0x0,0x9,0x0,0x0,0x9,0x0,0x0,0x0,0x9,0x0,0x0,0x0,0x0,0x0,0x9,0x0,0x0};
ubyte line5[32] = {0x0,0x0,0x0,0x1,0x0,0x0,0x5,0x6,0x6,0x2,0x0,0x0,0x0,0x1,0x0,0x0,0x1,0x0,0x0,0x1,0x0,0x4,0x6,0x7,0x2,0x0,0x4,0x6,0x6,0x3,0x0,0x0};

ubyte* lines[5] = { line1, line2, line3, line4, line5 };

ubyte skill_level = 5;

char levelY = 12;
void print_skill_levels()
{
    for (char i = 1; i <= 9; i++)
    {
        char x = 6 + 2*i;
        screen_print_char(x, levelY, '0' + i);
        char attr = ZX_BLK_YEL;
        if (i == skill_level)
            attr = ZX_YEL_BLK;
        screen_print_attr(x, levelY, attr);
    }
}

void enter_menu_loop()
{
    screen_border_set(ZX_BLK);
    screen_clear_bmap(0x00);
    screen_clear_attr(ZX_BLK_YEL);

    char titleY = 2;
    for (char i = 0; i < 5; i++)
    {
        ubyte* src = lines[i];
        ubyte* dst = (ubyte*) screen_get_bmap_addr(0, titleY + i);
        for (char j = 0; j < 32; j++)
        {
            param0w = dst++;
            param0b = *src++;
            if (param0b)
                param_draw_tile();
        }
    }
    for (char i = 0; i < 5; i++)
    {
        ubyte* src = lines[i];
        ubyte* dst = (ubyte*) screen_get_attr_addr(0, titleY + i);
        for (char j = 0; j < 32; j++)
        {
            if (*src++)
                *dst++ = ZX_WHI_RED;
            else
                dst++;
        }
    }

    char* msg;

    msg = "               Select skill level using keys 1 - 9                ";
    text_print(msg, 0, levelY - 2, ZX_BLK, ZX_WHI);
    text_print("easy", 9, levelY, ZX_BLK, ZX_YEL);
    text_print("hard", 53, levelY, ZX_BLK, ZX_YEL);

    print_skill_levels();


    char startY = 16;
    msg = "                    Press ENTER to start game                     ";
    text_print(msg, 0, startY, ZX_BLK, ZX_WHI);

    char bottomY = 20;
    //                  1         2         3         4         5         6
    //     0123456666789012345678901234567890123456789012345678901234567890123
    msg = "   Developed in 2020     for     1982 Sinclair ZX Spectrum 48K    ";
    text_print(msg, 0, bottomY, ZX_BLK, ZX_YEL);
    msg = "   by  David  Crespo             https://github.com/dcrespo3d/    ";
    text_print(msg, 0, bottomY+1, ZX_BLK, ZX_WHI);


    while (1) {
        HALT;
        keyb_read();
        if      (key1down()) { skill_level = 1; print_skill_levels(); }
        else if (key2down()) { skill_level = 2; print_skill_levels(); }
        else if (key3down()) { skill_level = 3; print_skill_levels(); }
        else if (key4down()) { skill_level = 4; print_skill_levels(); }
        else if (key5down()) { skill_level = 5; print_skill_levels(); }
        else if (key6down()) { skill_level = 6; print_skill_levels(); }
        else if (key7down()) { skill_level = 7; print_skill_levels(); }
        else if (key8down()) { skill_level = 8; print_skill_levels(); }
        else if (key9down()) { skill_level = 9; print_skill_levels(); }
        else if (keyENTdown()) break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Tetris for ZX Spectrum 48K
//
// (C) 2019, 2020 David Crespo - https://github.com/dcrespo3d/zx-spectrum-tetris
//
////////////////////////////////////////////////////////////////////////////////
#include "frame.h"

extern ubyte frame_corner_tl, frame_corner_tr, frame_corner_bl, frame_corner_br;
extern ubyte frame_segment_lr, frame_segment_tb;

void draw_frame(char x, char w, char y, char h, char attr)
{
    char x1, y1, x2, y2, i;
    x1 = x;
    y1 = y;
    x2 = x + w - 1;
    y2 = y + h - 1;

    uword bmap_addr, attr_addr;

    screen_print_user(x1, y1, &frame_corner_tl);
    screen_print_user(x2, y1, &frame_corner_tr);
    screen_print_user(x2, y2, &frame_corner_br);
    screen_print_user(x1, y2, &frame_corner_bl);

    attr_addr = screen_get_attr_addr(x1, y1);
    for (i = 0; i < w; i++) *((ubyte*)(attr_addr++)) = attr;
    attr_addr = screen_get_attr_addr(x1, y2);
    for (i = 0; i < w; i++) *((ubyte*)(attr_addr++)) = attr;

    char ww = w - 2;
    char hh = h - 2;

    bmap_addr = screen_get_bmap_addr(x1+1, y1);
    for (i = 0; i < ww; i++) screen_print_user_raw(bmap_addr++, &frame_segment_lr);
    bmap_addr = screen_get_bmap_addr(x1+1, y2);
    for (i = 0; i < ww; i++) screen_print_user_raw(bmap_addr++, &frame_segment_lr);

    char y1a = y1 + 1;

    for (i = 0; i < hh; i++) {
        bmap_addr = screen_get_bmap_addr(x1, y1a+i);
        screen_print_user_raw(bmap_addr, &frame_segment_tb);
        attr_addr = screen_get_attr_addr(x1, y1a+i);
        *((ubyte*)(attr_addr)) = attr;
    }
    for (i = 0; i < hh; i++) {
        bmap_addr = screen_get_bmap_addr(x2, y1a+i);
        screen_print_user_raw(bmap_addr, &frame_segment_tb);
        attr_addr = screen_get_attr_addr(x2, y1a+i);
        *((ubyte*)(attr_addr)) = attr;
    }
}

__asm
    ._frame_corner_tl
        DEFB %00000000
        DEFB %00000111
        DEFB %00011111
        DEFB %00111111
        DEFB %00111111
        DEFB %01111111
        DEFB %01111111
        DEFB %01111111
    ._frame_corner_tr
        DEFB %00000000
        DEFB %11100000
        DEFB %11111000
        DEFB %11111100
        DEFB %11111100
        DEFB %11111110
        DEFB %11111110
        DEFB %11111110
    ._frame_corner_bl
        DEFB %01111111
        DEFB %01111111
        DEFB %01111111
        DEFB %00111111
        DEFB %00111111
        DEFB %00011111
        DEFB %00000111
        DEFB %00000000
    ._frame_corner_br
        DEFB %11111110
        DEFB %11111110
        DEFB %11111110
        DEFB %11111100
        DEFB %11111100
        DEFB %11111000
        DEFB %11100000
        DEFB %00000000
    ._frame_segment_lr
        DEFB %00000000
        DEFB %11111111
        DEFB %11111111
        DEFB %11111111
        DEFB %11111111
        DEFB %11111111
        DEFB %11111111
        DEFB %00000000
    ._frame_segment_tb
        DEFB %01111110
        DEFB %01111110
        DEFB %01111110
        DEFB %01111110
        DEFB %01111110
        DEFB %01111110
        DEFB %01111110
        DEFB %01111110
__endasm


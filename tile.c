////////////////////////////////////////////////////////////////////////////////
//
// Tetris for ZX Spectrum 48K
//
// (C) 2019, 2020 David Crespo - https://github.com/dcrespo3d/zx-spectrum-tetris
//
////////////////////////////////////////////////////////////////////////////////
#include "params0.h"
#include "tile.h"

// param0w: addr
// param0b: tile [0000BRLT]
void param_draw_tile()
{
    __asm
        PUSH BC
        PUSH DE
        PUSH HL

        LD   HL, (_param0w)

        LD   A, (_param0b)
        AND  A, %00000001
        LD   DE, BLRT_TILE_TB
        ADD  A, E
        LD   E, A

        LD   A, (DE)
        LD  (HL), A
        INC  H

        LD   A, (_param0b)
        AND  A, %00000110
        RRCA
        LD   DE, BLRT_TILE_RL
        ADD  A, E
        LD   E, A

        LD   B, 6
    PARAM_DRAW_TILE_LOOP:    
        LD   A, (DE)
        LD  (HL), A
        INC  H
        DJNZ PARAM_DRAW_TILE_LOOP

        LD   A, (_param0b)
        AND  A, %00001000
        RRCA
        RRCA
        RRCA
        LD   DE, BLRT_TILE_TB
        ADD  A, E
        LD   E, A

        LD   A, (DE)
        LD  (HL), A
        
        POP  HL
        POP  DE
        POP  BC
    __endasm
}

__asm
align 256
    BLRT_TILE_TB:
        DEFB %00000000
    BLRT_TILE_RL:
        DEFB %01111110
        DEFB %11111110
        DEFB %01111111
        DEFB %11111111
__endasm

// param0w: addr
void param_clear_tile()
{
    __asm
        PUSH BC
        PUSH HL

        LD   HL, (_param0w)
        LD   B, 8
        LD   A, 0
    PARAM_CLEAR_TILE_LOOP:
        LD  (HL), A
        INC  H
        DJNZ PARAM_CLEAR_TILE_LOOP
        
        POP  HL
        POP  DE
    __endasm
}


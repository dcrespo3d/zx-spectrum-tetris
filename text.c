////////////////////////////////////////////////////////////////////////////////
//
// Tetris for ZX Spectrum 48K
//
// (C) 2019, 2020 David Crespo - https://github.com/dcrespo3d/zx-spectrum-tetris
//
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>

static char color_trans[8] = { 0, 1, 4, 5, 2, 3, 14, 7 };

void text_print(char* msg, char col, char row, char paper, char ink)
{
    // see https://github.com/z88dk/z88dk/wiki/Platform---Sinclair-ZX-Spectrum
    // and https://github.com/z88dk/z88dk/wiki/Classic-GenericConsole
    // and https://www.z88dk.org/forum/viewtopic.php?id=4395
    row += 0x20; // add displacement
    //col <<= 1; // double column coordinate for 32 column mode
    col += 0x20; // add displacement
    paper = color_trans[paper & 7];
    ink   = color_trans[ink   & 7];
    printf("\x10%c\x11%c\x16%c%c%s", ink, paper, row, col, msg);
}












/*

#include <stdio.h>
#include <font/font.h>
#include <sys/ioctl.h>

void print(const char *name, void *font);
void print64(const char *name, void *font);

int amain() {

    char row = 10;
    char col = 15;
    row += 0x20; // add displacement
    col <<= 1; // double column coordinate for 32 column mode
    col += 0x20; // add displacement
    printf("\x16%c%c%s", row, col, "jarl");
    getchar();


    printf("%c%c\n",1,32);
    print("default", (void *)15616);
    print("font_8x8_bbc_system", &font_8x8_bbc_system);
    print("font_8x8_c64_system", &font_8x8_c64_system);
    getchar();
    printf("%c\n",12);
    print("font_8x8_clairsys", &font_8x8_clairsys);
    print("font_8x8_clairsys_bold", &font_8x8_clairsys_bold);
    print("font_8x8_cpc_system", &font_8x8_c64_system);
    getchar();
    printf("%c\n",12);
    print("font_8x8_vga_rom", &font_8x8_vga_rom);
    print("font_8x8_zx_system", &font_8x8_zx_system);

    getchar();
    printf("%c\n",12);
    printf("%c%c\n",1,64);
    print64("font_4x8_64_minix", &font_4x8_64_minix);
    print64("font_4x8_64_nbot", &font_4x8_64_nbot);
    print64("font_4x8_64_omni1", &font_4x8_64_omni1);
    print64("font_4x8_64_omni2", &font_4x8_64_omni2);
    print64("font_4x8_64_owen", &font_4x8_64_owen);
    getchar();
    printf("%c\n",12);
    print64("font_4x8_80columns", &font_4x8_80columns);
    print64("font_4x8_color80", &font_4x8_color80);
    print64("font_4x8_default", &font_4x8_default);
    print64("font_4x8_highspeed80", &font_4x8_highspeed80);
    print64("font_4x8_screen80", &font_4x8_screen80);
    getchar();

}


void print(const char *name, void *font)
{
    console_ioctl(IOCTL_GENCON_SET_FONT32, &font);
    printf("This is font %s\n",name);

    for ( int i = 32 ; i < 128; i++ ) {
        printf("%c",i);
    }
    printf("\n\n");
}
void print64(const char *name, void *font)
{
    console_ioctl(IOCTL_GENCON_SET_FONT64, &font);
    printf("This is font %s\n",name);

    for ( int i = 32 ; i < 128; i++ ) {
        printf("%c",i);
    }
    printf("\n\n");
}



*/
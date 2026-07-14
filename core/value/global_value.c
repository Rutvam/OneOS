// Debut global_value.c
#include "global_value.h"

#include <stdint.h>

uint8_t kernel_vBig;
uint8_t kernel_vMid;
uint8_t kernel_vLit;

struct CURSOR cursor = {
    .skin = '_',
    .temp_char = ' ',
    .temp_color = 0x0F,
    .position = 0,
    .affiche = 0
};

struct CHARA char_A = {
    .LIGNE1 = 0x00, // 00000000
    .LIGNE2 = 0x3C, // 00111100
    .LIGNE3 = 0x66, // 01100110
    .LIGNE4 = 0x66, // 01100110
    .LIGNE5 = 0x7E, // 01111110
    .LIGNE6 = 0x66, // 01100110
    .LIGNE7 = 0x66, // 01100110
    .LIGNE8 = 0x00, // 00000000
};

struct CHARA char_B = {
    .LIGNE1 = 0x00, // 00000000
    .LIGNE2 = 0x7C, // 01111100
    .LIGNE3 = 0x66, // 01100110
    .LIGNE4 = 0x7C, // 01111100
    .LIGNE5 = 0x66, // 01100110
    .LIGNE6 = 0x66, // 01100110
    .LIGNE7 = 0x7C, // 01111100
    .LIGNE8 = 0x00, // 00000000
};

struct CHARA char_C = {
    .LIGNE1 = 0x00, // 00000000
    .LIGNE2 = 0x3C, // 00111100
    .LIGNE3 = 0x66, // 01100110
    .LIGNE4 = 0x60, // 01100000
    .LIGNE5 = 0x60, // 01100000
    .LIGNE6 = 0x66, // 01100110
    .LIGNE7 = 0x3C, // 00111100
    .LIGNE8 = 0x00, // 00000000
};

// Debut global_value.c

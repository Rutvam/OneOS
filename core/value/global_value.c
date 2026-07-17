// Debut global_value.c
#include "global_value.h"

#include <stdint.h>

uint8_t kernel_vBig;
uint8_t kernel_vMid;
uint8_t kernel_vLit;

// Struct du curseur defini de base

struct CURSOR cursor = {
    .skin = '_',
    .temp_char = ' ',
    .temp_color = 0x0F,
    .position = 0,
    .affiche = 0
};



// Struct de chaque touch

struct TOUCHE MAJ_L = {
    .value = 0,
    .SCC_press = 0x2A,
    .SCC_release = 0xAA
};
struct TOUCHE MAJ_R = {
    .value = 0,
    .SCC_press = 0x36,
    .SCC_release = 0xB6
};
struct TOUCHE CTRL = {
    .value = 0,
    .SCC_press = 0x1D,
    .SCC_release = 0x9D
};
struct TOUCHE etendue = {
    .value = 0,
    .SCC_press = 0x0E
};
struct TOUCHE ALT_L = {
    .value = 0,
    .SCC_press = 0x38,
    .SCC_release = 0xB8
};

// Debut global_value.c

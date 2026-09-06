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

struct TOUCHE FLECHE_L = {
    .value = 0,
    .SCC_press = 0x4B,
    .SCC_release = 0xCB,
};

struct TOUCHE FLECHE_R = {
	.value = 0,
	.SCC_press = 0x4D,
	.SCC_release = 0xCD,
};

struct TOUCHE FLECHE_T = {
	.value = 0,
	.SCC_press = 0x48,
	.SCC_release = 0xC8,
};

struct TOUCHE FLECHE_B = {
    .value = 0,
    .SCC_press = 0x50,
    .SCC_release = 0xD0,
};

/*
 ↑ : E0 48 / E0 C8
 ↓ : E0 50 / E0 D0
 ← : E0 4B / E0 CB
 → : E0 4D / E0 CD
*/


struct KNOPF pong = {
	.value = 0,
};
struct KNOPF shell = {
	.value = 1,
};
struct KNOPF shutdown = {
	.value = 0,
};


uint8_t screen[2][80][25] = {0x00};

// Debut global_value.c

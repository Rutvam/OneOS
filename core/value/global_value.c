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

// Debut global_value.c

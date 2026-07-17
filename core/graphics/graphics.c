#include <stdint.h>
#include "graphics.h"
#include "../value/global_value.h"

void putpixel(int x, int y, uint32_t color) {
    struct VBEInfo* vbe = (struct VBEInfo*) 0x9000;

    if (x < 0 || y < 0 || x >= vbe->XResolution || y >= vbe->YResolution)
        return;

    uint32_t* fb = (uint32_t*) vbe->PhysBasePtr;
    int pitch = vbe->pitch / 4;
    fb[y * pitch + x] = color;
}
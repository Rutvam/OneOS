#include <stdint.h>
#include "graphics.h"
#include "../core/value/global_value.h"

void putPixel(int x, int y, uint32_t color) {
    struct VBEInfo* vbe = (struct VBEInfo*) 0x9000;

    if (x < 0 || y < 0 || x >= vbe->XResolution || y >= vbe->YResolution)
        return;

    uint32_t* fb = (uint32_t*) vbe->PhysBasePtr;
    int pitch = vbe->pitch / 4;
    fb[y * pitch + x] = color;
    return;
}

void analizeScreen() {
    struct VBEInfo* vbe = (struct VBEInfo*) 0x9000;
    uint32_t* fb = (uint32_t*) vbe->PhysBasePtr;
    int pitch = vbe->pitch / 4;
	for (int x = 0; x < 1920; x++) {
		for (int y = 0; y < 1920; y++) {
			uint32_t color = screen[1][x][y]*16777216 + screen[2][x][y]*65536 + screen[3][x][y]*256 + screen[4][x][y];
			if (!(color == fb[y * pitch + x])) {
				screen[0][x][y] = 1;
			}
		}	
	}
	return;
}

void putScreen() {
	// screen [0] = change or not
	// screen [1] = Bonus
	// screen [2] = Red
	// screen [3] = Green
	// screen [4] = Blue
	analizeScreen();
	for (int x = 0; x < 1920; x++) {
		for (int y = 0; y < 1920; y++) {
			if (screen[0][x][y]) {
				int color = screen[1][x][y]*16777216 + screen[2][x][y]*65536 + screen[3][x][y]*256 + screen[4][x][y];
				putPixel(x, y, color);
			}
		}	
	}
	return;
}



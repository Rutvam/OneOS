// Debut global_value.h
#ifndef GLOBAL_VALUE_H
#define GLOBAL_VALUE_H

#include <stdint.h>

extern uint8_t kernel_vBig;
extern uint8_t kernel_vMid;
extern uint8_t kernel_vLit;

typedef struct VBEInfo {
    uint16_t attributes;
    uint8_t  winA, winB;
    uint16_t granularity;
    uint16_t winsize;
    uint16_t segmentA, segmentB;
    uint32_t realFctPtr;
    uint16_t pitch;

    uint16_t XResolution;
    uint16_t YResolution;
    uint8_t  XCharSize;
    uint8_t  YCharSize;
    uint8_t  numberOfPlanes;
    uint8_t  bitsPerPixel;
    uint8_t  numberOfBanks;
    uint8_t  memoryModel;
    uint8_t  bankSize;
    uint8_t  numberOfImagePages;
    uint8_t  reserved1;

    uint8_t  redMaskSize;
    uint8_t  redFieldPosition;
    uint8_t  greenMaskSize;
    uint8_t  greenFieldPosition;
    uint8_t  blueMaskSize;
    uint8_t  blueFieldPosition;
    uint8_t  reservedMaskSize;
    uint8_t  reservedFieldPosition;
    uint8_t  directColorModeInfo;

    uint32_t PhysBasePtr;
    uint32_t OffScreenMemOffset;
    uint16_t OffScreenMemSize;
    uint8_t  reserved2[206];
} VBEInfo;


struct CURSOR {
	char skin;
	char temp_char;
	int temp_color;
	int position;
	int affiche;
};

extern struct CURSOR cursor;

struct CHARA {
	int LIGNE1;
	int LIGNE2;
	int LIGNE3;
	int LIGNE4;
	int LIGNE5;
	int LIGNE6;
	int LIGNE7;
	int LIGNE8;
};

extern struct CHARA char_A;
extern struct CHARA char_B;
extern struct CHARA char_C;

#endif
// Fin global_value.h

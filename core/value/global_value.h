// Debut global_value.h
#ifndef GLOBAL_VALUE_H
#define GLOBAL_VALUE_H

#include <stdint.h>

extern uint8_t kernel_vBig;
extern uint8_t kernel_vMid;
extern uint8_t kernel_vLit;

struct CURSOR {
	char skin;
	char temp_char;
	int temp_color;
	int position;
	int affiche;
};

extern struct CURSOR cursor;

#endif
// Fin global_value.h

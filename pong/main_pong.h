#include <stdint.h>
#ifndef MAIN_PONG_H
#define MAIN_PONG_H


#include "../core/value/global_value.h"
#include "../core/keyboard/keyboard.h"
#include "../core/function/function.h"


struct SPRITE {
	char skin;
	uint8_t value;
	uint8_t xc; // X Coordination
	uint8_t yc; // Y Coordination
	uint8_t xd; // X Direction 
	uint8_t yd; // Y Direction 
};
extern struct SPRITE ball;
extern struct SPRITE player1;
extern struct SPRITE player2;

char analyze_field (int field_value);
void init_sprites();
void print_map ();
int main_pong();

#endif

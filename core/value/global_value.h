// Debut global_value.h
#ifndef GLOBAL_VALUE_H
#define GLOBAL_VALUE_H

#include <stdint.h>

extern uint8_t kernel_vBig;
extern uint8_t kernel_vMid;
extern uint8_t kernel_vLit;

// structure du curseur

struct CURSOR {
	char skin;
	char temp_char;
	int temp_color;
	int position;
	int affiche;
};

extern struct CURSOR cursor;



struct TOUCHE {
    int value;
    int SCC_press;
    int SCC_release;
};
extern struct TOUCHE MAJ_L;
extern struct TOUCHE MAJ_R;
extern struct TOUCHE CTRL;
extern struct TOUCHE etendue;
extern struct TOUCHE ALT_L;
extern struct TOUCHE FLECHE_L;
extern struct TOUCHE FLECHE_R;
extern struct TOUCHE FLECHE_T;
extern struct TOUCHE FLECHE_B;
extern uint8_t screen[2][80][25];

struct KNOPF
{
	char name[15];
	uint8_t value;
};
extern struct KNOPF pong;
extern struct KNOPF shell;
extern struct KNOPF shutdown;


#endif
// Fin global_value.h

// Debut global_value.h
#include <stdint.h>
#include "../graphics/graphics.h"
#ifndef GLOBAL_VALUE_H
#define GLOBAL_VALUE_H

struct KNOPF
{
	char *name;
	uint8_t value;
};

struct KNOPF_VARIABLE
{
	struct KNOPF pong;
	struct KNOPF shell;
	struct KNOPF shutdown;
};

struct KEY
{
	uint8_t value;
	uint8_t SCC_press;
	uint8_t SCC_release;
	uint8_t need_ethendu;
};

struct KEY_VARIABLE
{
	struct KEY ENTER;
	struct KEY TAB;
	struct KEY CAPS_LOCK;
	struct KEY DELET;
	struct KEY MAJ_L;
	struct KEY MAJ_R;
	struct KEY CTRL_L;
	struct KEY CTRL_R;
	struct KEY ALT_L;
	struct KEY ALT_R;
	struct KEY FLECHE_L;
	struct KEY FLECHE_R;
	struct KEY FLECHE_T;
	struct KEY FLECHE_B;
};

struct ETHENDU
{
	uint8_t value;
	uint8_t SCC_prefix;
};

struct GPU_CONFIGS
{
	uint32_t *FB;
	uint32_t FBSize;
	uint32_t width;
	uint32_t height;
	uint32_t pitch;
};

struct VARIABLE
{
	struct GPU_CONFIGS gpu;
	struct ETHENDU ethendu;
	struct KNOPF_VARIABLE knopf;
	struct KEY_VARIABLE key;
	struct {const char *shutdown; const char *exit; const char *clear; const char *clist;} command;
	struct {char *problem; int value;} status;
	uint32_t police[128][13][8];
	uint32_t video[4000];
	int cursor_position;
};

extern struct VARIABLE var;

#endif
// Fin global_value.h

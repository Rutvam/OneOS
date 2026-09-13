#include <stdint.h>
#include <stdarg.h>
#include <efi.h>
#ifndef FUNCTION_H
#define FUNCTION_H


struct PRINT_FUNCTION
{
	void (*basic)(const char *text);
	void (*outb)(const char *text, ...);
};

struct CMP_FUNCTION
{
	int (*string)(const char *first_string, const char *second_string);
	int (*key)(uint8_t value, uint8_t SCC_press, uint8_t SCC_release, uint8_t need_ethendu, uint8_t last_scancode);
};

struct INIT
{
	void (*function)(int is_qemu);
	void (*var)(UINT64 framebuffer_base);
};

struct SCREEN_FUNCTION
{
	void (*analyse)(void);
};

struct MY_FUNCTION
{
	struct PRINT_FUNCTION print;
	struct CMP_FUNCTION cmp;
	struct INIT init;
	struct SCREEN_FUNCTION screen;
	void (*clear)(void);
	void (*outb)(unsigned short port, unsigned char value);
};

extern struct MY_FUNCTION my;

void init_function(int is_qemu);

#endif

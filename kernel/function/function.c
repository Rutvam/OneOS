#include <stdarg.h>
#include <stdarg.h>
#include <efi.h>
#include "function.h"
#include "../../data/global_value.h"
#include "../../graphics/graphics.h"
/*
void selection (uint8_t value, int place)
{
	char* video = (char*)0xB8000;
	switch (value)
	{
		case 1:
			video[place] = '>';
			// screen[0][40][2] = '>';
			video[place + 1] = 0x02;
			// screen[1][40][2] = 0x0F;
			break;
		case 0:
			video[place] = ' ';
			video[place + 1] = 0x02;
			break;
		default:
			video[place] = '?';
			video[place + 1] = 0x02;
			break;
	}
}

void save_screen ()
{
	char* video = (char*)0xB8000;
	for (int x = 0; x < 80; x++)
	{
		for(int y = 0; y < 25; y++)
		{
			screen[0][x][y] = video[x * 2 + y * 160];
			screen[1][x][y] = video[1+(x * 2 + y * 160)];
		}
	}
}

void refrech_screen ()
{
	char* video = (char*)0xB8000;
	selection(pong.value, 522);
	selection(shell.value, 682);
	selection(shutdown.value, 842);
	for (int x = 0; x < 80; x++)
	{
		for(int y = 0; y < 25; y++)
		{
			if (screen[0][x][y] != video[x * 2 + y * 160])
			{
				video[x * 2 + y * 160] = screen[0][x][y];
			}
			
			if (screen[1][x][y] != video[(x * 2 + y * 160)+1])
			{
				video[1+(x * 2 + y * 160)] = screen[1][x][y];
			}
		}
	}

	save_screen();
}

void print_main_menu ()
{
	my.clear();

	pong.name[0] = 'P';
	pong.name[1] = 'O';
	pong.name[2] = 'N';
	pong.name[3] = 'G';
	pong.name[4] = 0x00;

	shell.name[0] = 'S';
	shell.name[1] = 'H';
	shell.name[2] = 'E';
	shell.name[3] = 'L';
	shell.name[4] = 'L';
	shell.name[5] = 0x00;

	shutdown.name[0] = 's';
	shutdown.name[1] = 'h';
	shutdown.name[2] = 'u';
	shutdown.name[3] = 't';
	shutdown.name[4] = 'd';
	shutdown.name[5] = 'o';
	shutdown.name[6] = 'w';
	shutdown.name[7] = 'n';
	shutdown.name[8] = 0x00;
	char title[12] = "GAMEBOY OS";

	int j = 0;
	for (int i = 384; title[j] != 0x00; i += 2)
	{
		video[i + 4] = title[j];
		video[i + 5] = 0x02;
		j++;
	}

	video[198] = 0xC9; // coins superieur gauche
	video[199] = 0x02;

	video[280] = 0xBB; // coins superieur droite
	video[281] = 0x02;

	// screen[0][19][1] = 0xC9;
	// screen[1][19][1] = 0x0F;

	// screen[0][60][1] = 0xBB;
	// screen[1][60][1] = 0x0F;

	for (int i = 200; i < 280; i += 2)
	{
		video[i] = 0xCD; // bordure superieur
		video[i+1] = 0x02;
	}

	for (int i = 1; i < 22; i++)
	{
		video[280 + (160 * i)] = 0xBA; // bordure droit
		video[281 + (160 * i)] = 0x02;
	}

	for (int i = 1; i < 22; i++)
	{
		video[198 + (160 * i)] = 0xBA; // bordure gauche
		video[199 + (160 * i)] = 0x02;
	}

	video[3718] = 0xC8;
	video[3719] = 0x02;

	for (int i = 3720; i < 3800; i += 2)
	{
		video[i] = 0xCD;
		video[i + 1] = 0x02;
	}

	video[3800] = 0xBC;
	video[3801] = 0x02;
	selection(pong.value, 522);
	j = 0;
	for (int i = 522; pong.name[j] != 0x00; i += 2)
	{
		video[i + 4] = pong.name[j];
		video[i + 5] = 0x02;
		j++;
	}

	selection(shell.value, 682);
	j = 0;
	for (int i = 682; shell.name[j] != 0x00; i += 2)
	{
		video[i + 4] = shell.name[j];
		video[i + 5] = 0x02;
		j++;
	}

	selection(shutdown.value, 842);

	j = 0;
	for (int i = 842; shutdown.name[j] != 0x00; i += 2)
	{
		video[i + 4] = shutdown.name[j];
		video[i + 5] = 0x02;
		j++;
	}
	

	// refrech_screen();
	save_screen();
}
*/

extern void outb (unsigned short port, unsigned char value);

void clear()
{
	my.print.outb("\033[2J\033[H");
	for (int i = 0; i < (int)var.gpu.FBSize; i++)
	{
		var.gpu.FB[i] = 0x000000;
	}
	return;
}

int string(const char *first_string, const char *second_string)
{
	while (*first_string && *second_string && *first_string == *second_string)
	{
		first_string++;
		second_string++;
	}
	return (unsigned char)*first_string - (unsigned char)*second_string;
}

int key(uint8_t value, uint8_t SCC_press, uint8_t SCC_release, uint8_t need_ethendu, uint8_t last_scancode)
{
	if (var.ethendu.value == need_ethendu)
	{
		if (SCC_press == last_scancode) {
			return 1;
		} else if (SCC_release == last_scancode) {
			return 0;
		}
	}
	return value;
}

void print(const char* text)
{
	my.print.outb(text);
//     char* video = (char*)0xB8000;
//     int j = 0;
// 
//     while (text[j] != 0 && cursor.position < 4000)
//     {
//         if (text[j] == '\n') {
//             cursor.position = ((cursor.position) / 160 + 1) * 160;
//             j++;
//             continue;
//         }
// 
//         video[cursor.position] = text[j];
//         video[cursor.position + 1] = 0x0F;   // blanc sur noir
//         cursor.position += 2;
//         j++;
//         
//         if (cursor.position >= 3999)
//         {
//         	for (int k = 160; k <= 3999; k++)
//         	{
//         		video[k-160] = video[k];
//         	}
// 
//         	for (int k = 3840; k < 3999; k += 2)
//         	{
//         		video[k] = ' ';
//         		video[k+1] = 0x0F;
//         	}
// 
//         	cursor.position = 3840;
// 		}
// 	}
	return;
}

void print_outb(const char *text, ...)
{
	va_list args;
	va_start(args, text);

	for (int i = 0; text[i] != 0x00; i++)
	{
		if (text[i] == '%' && text[i + 1] == 'c') {
			char caracter = va_arg(args, char);
			my.outb(0x3F8, caracter);
			i++;
		} else if (text[i] == '%' && text[i + 1] == 'd') {
			int value = va_arg(args, int);
			char caracter;
			if (value > 10) {caracter = value + '0';}
			my.outb(0x3F8, caracter);
			i++;
		} else if (text[i] == '%' && text[i + 1] == 's') {
			char *string = va_arg(args, char *);
			my.print.outb(string);
			i++;
		} else if (text[i] == '%' && text[i + 1] == 'h') {
			int value = va_arg(args, int);
			char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
			char caracter = hex[value];
			my.outb(0x3F8, caracter);
			i++;
		} else {
			my.outb(0x3F8, text[i]);
		}
	}

	va_end(args);
	return;
}

void NO_print(const char *text, ...)
{
	(void)text;
	return;
}

void init_var(UINT64 framebuffer_base)
{
	var.gpu.FB = (uint32_t *)(uintptr_t)framebuffer_base;
	var.key.TAB.value = 0;
	var.key.TAB.SCC_press = 0x0F;
	var.key.TAB.SCC_release = 0x8F;
	var.key.ALT_L.value = 0;
	var.key.ALT_L.SCC_press = 0x38;
	var.key.ALT_L.SCC_release = 0xB8;
	var.key.ALT_L.need_ethendu = 0;
	var.key.ALT_R.value = 0;
	var.key.ALT_R.SCC_press = 0x38;
	var.key.ALT_R.SCC_release = 0xB8;
	var.key.ALT_R.need_ethendu = 1;
	var.key.DELET.value = 0;
	var.key.DELET.SCC_press = 0x53;
	var.key.DELET.SCC_release = 0xD3;
	var.key.DELET.need_ethendu = 1;
	var.key.ENTER.value = 0;
	var.key.ENTER.SCC_press = 0x1C;
	var.key.ENTER.SCC_release = 0x9C;
	var.key.ENTER.need_ethendu = 0;
	var.key.MAJ_L.value = 0;
	var.key.MAJ_L.SCC_press = 0x2A;
	var.key.MAJ_L.SCC_release = 0xAA;
	var.key.MAJ_L.need_ethendu = 0;
	var.key.MAJ_R.value = 0;
	var.key.MAJ_R.SCC_press = 0x36;
	var.key.MAJ_R.SCC_release = 0xB6;
	var.key.MAJ_R.need_ethendu = 0;
	var.key.CTRL_L.value = 0;
	var.key.CTRL_L.SCC_press = 0x1D;
	var.key.CTRL_L.SCC_release = 0x9D;
	var.key.CTRL_L.need_ethendu = 0;
	var.key.CTRL_R.value = 0;
	var.key.CTRL_R.SCC_press = 0x1D;
	var.key.CTRL_R.SCC_release = 0x9D;
	var.key.CTRL_R.need_ethendu = 1;
	var.key.FLECHE_L.value = 0;
	var.key.FLECHE_L.SCC_press = 0x4B;
	var.key.FLECHE_L.SCC_release = 0xCB;
	var.key.FLECHE_L.need_ethendu = 1;
	var.key.FLECHE_R.value = 0;
	var.key.FLECHE_R.SCC_press = 0x4D;
	var.key.FLECHE_R.SCC_release = 0xCD;
	var.key.FLECHE_R.need_ethendu = 1;
	var.key.FLECHE_T.value = 0;
	var.key.FLECHE_T.SCC_press = 0x48;
	var.key.FLECHE_T.SCC_release = 0xC8;
	var.key.FLECHE_T.need_ethendu = 1;
	var.key.FLECHE_B.value = 0;
	var.key.FLECHE_B.SCC_press = 0x50;
	var.key.FLECHE_B.SCC_release = 0xD0;
	var.key.FLECHE_B.need_ethendu = 1;
	var.key.CAPS_LOCK.value = 0;
	var.key.CAPS_LOCK.SCC_press = 0x3A;
	var.key.CAPS_LOCK.SCC_release = 0xBA;
	var.key.CAPS_LOCK.need_ethendu = 0;
	var.knopf.pong.name = "Pong";
	var.knopf.pong.value = 0;
	var.knopf.shell.name = "Shell";
	var.knopf.shell.value = 1;
	var.knopf.shutdown.name = "Shutdown";
	var.knopf.shutdown.value = 0;
	var.command.shutdown = "shutdown";
	var.command.exit = "exit";
	var.command.clear = "clear";
	var.command.clist = "clist";
	var.ethendu.value = 0;
	var.ethendu.SCC_prefix = 0xE0;
	var.cursor_position = 0;
}

struct MY_FUNCTION my;

void init_function(int is_qemu)
{
	my.init.var = init_var;
	my.clear = clear;
	my.outb = outb;
	my.cmp.string = string;
	my.cmp.key = key;
	my.print.basic = print;
	my.screen.analyse = screen_analyze;
	if (is_qemu) {
		my.print.outb = print_outb;
	} else {
		my.print.outb = NO_print;
	}
	return;
}




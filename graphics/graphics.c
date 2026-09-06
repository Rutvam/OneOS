#include <stdint.h>
#include "./graphics.h"
#include "../core/value/global_value.h"
#include "../core/function/function.h"

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
	char* video = (char*)0xB8000;
	clear();

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

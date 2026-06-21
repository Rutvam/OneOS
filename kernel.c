#include "IDT.h"
#include "math.h"
#include "function.h"
extern void keyboard_handler_asm();

void clear()
{
	char* video_memory = (char*) 0xB8000;
	video_memory[0] = ' ';
	for (int i = 2; i <= 4000; i = i + 2)
	{
		video_memory[i] = ' ';
		video_memory[i-1] = 0x0F;
	}
}

void set(const char* text)
{
	char* video_memory = (char*) 0xB8000;
	int background_color = 0;
	int text_color = 15;
	int color = background_color * 16 + text_color * 1;
	int reset = 0;
	int j = 0;
	for (int i = 0; i <= 4000 && text[j] != 0x00; i = i + 2)
	{
		if (text[j] == '\033' && text[j+1] == '[' && (text[j+7] == 'm' || text[j+4] == 'm' || text[j+3] == 'm')) {
			if ('1' <= text[j+3] && text[j+3] <= '8' && text[j+2] == '3')
			{
				text_color = text[j+3] - '1';
			}
			if ('1' <= text[j+6] && text[j+6] <= '8' && text[j+5] == '3')
			{
				text_color = text[j+6] - '1';
			}
			if ('1' <= text[j+3] && text[j+3] <= '8' && text[j+2] == '4')
			{
				background_color = text[j+3] - '1';
			}
			if ('1' <= text[j+6] && text[j+6] <= '8' && text[j+5] == '4')
			{
				background_color = text[j+6] - '1';
			}
			if (text[j+2] == 0x00) {
				reset = 1;
				background_color = 0;
				text_color = 15;
			}

			if (background_color != 0 && text_color != 0) {
				j = j + 8;
			} else if (background_color != 0 || text_color != 0) {
				j = j + 5;
			} else if (reset) {
				j = j + 4;
			}
			continue;
		} else if (text[j] == '\n') {
			i = ((i/160)+1)*160;
			j++;
			continue;
		} else if (text[j] == '\t') {
			i += 4;
			j++;
			continue;
		}
		color = background_color * 16 + text_color * 1;
		video_memory[i] = text[j];
		video_memory[i + 1] = color;
		j++;
	}
}

// Cette fonction sera appelée à chaque fois que tu touches au clavier !
void keyboard_handler_c() {
    set("\033[32m[Clavier] Une touche a ete pressee !\033[0m\n");

    // LIGNE OBLIGATOIRE : On doit dire au contrôleur d'interruptions (le PIC)
    // qu'on a bien fini de traiter le signal, sinon il bloquera les prochaines touches.
    // Le port 0x20 reçoit le signal de fin (0x20).
    __asm__ __volatile__("outb %%al, %%dx" : : "a"(0x20), "d"(0x20));
}

int main()
{
    clear();
    set("\033[34mThe Kernel\033[0m\n");
    
    // 1. On configure la case 33 (clavier) de l'IDT avec notre code ASM
    set_idt_gate(33, (uint32_t)keyboard_handler_asm);
    
    // 2. On charge l'IDT dans le processeur
    init_idt();
    
    set("IDT chargee avec succes.\n");

    // 3. Par défaut, le processeur ignore les interruptions matérielles.
    // L'instruction assembleur "sti" (Set Interrupt Flag) lui dit : "Maintenant, écoute le clavier !"
    __asm__ __volatile__("sti");

    set("Interruptions activees ! Appuie sur une touche de ton clavier...\n");

    while(1);
    return 0;
}

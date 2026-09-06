#include <stdint.h>
#include "../core/idt/IDT.h"
#include "../core/value/global_value.h"
#include "../core/keyboard/keyboard.h"
#include "../core/function/function.h"
#include "../shutdown/main_shutdown.h"

char memory[300] = {0x00};

void erase_memory(void)
{
	for (int i = 0; i < 300; i++)
	{
		memory[i] = 0;
	}
}

static inline void outb (unsigned short port, unsigned char value)
{
	__asm__ volatile (".intel_syntax noprefix\n" "out dx, al\n" ".att_syntax\n" : : "d"(port), "a"(value));
}

static inline unsigned char inb (unsigned short port)
{
	unsigned char value;
	__asm__ volatile (".intel_syntax noprefix\n" "in al, dx\n" ".att_syntax\n" : "=a"(value) : "d"(port));
	return value;
}

void cursor_enable (void)
{
	outb(0x3D4, 0x0A);
	unsigned char cursor_start = inb(0x3D5);
	outb(0x3D5, cursor_start & 0xC0);

	outb(0x3D4, 0x0B);
	unsigned char cursor_end = inb(0x3D5);
	outb(0x3D5, cursor_end & 0xE0);
}

void cursor_move (void)
{
	unsigned short pos = cursor.position/2+80;
	outb(0x3D4, 0x0F);
	outb(0x3D5, pos & 0xFF);

	outb(0x3D4, 0x0E);
	outb(0x3D5, (pos >> 8) & 0xFF);
}

struct {
	const char shutdown[9];
	const char exit[5];
	const char clear[6];
	const char clist[6];
} command = {
	.shutdown = "shutdown",
	.exit = "exit",
	.clear = "clear",
	.clist = "clist",
};

int main_shell ()
{
	cursor.position = 0;
	int nb_char = 0;
	char* video_memory = (char*)0xB8000;

	char PROMPT[4] = {'>', '>', ' ', 0x00};

    // 1. On prépare l'affichage
	clear();
    print(PROMPT);
	
    // 2. Configuration matérielle (Une seule fois !)
    pic_remap();
    set_idt_gate(33, (uint32_t)(uintptr_t)keyboard_handler_asm);
    init_idt();

    // 3. On ouvre les vannes du clavier
    __asm__ __volatile__("sti");
    int ALT_GR = 0;
    int CTRL_L = 0;
    int CTRL_R = 0;
    cursor_enable();
    // 4. Boucle de repos (Le CPU attend sagement ici)
    while (1)
    {
		cursor_move();
        if (last_scancode) {

            if ((last_scancode == MAJ_L.SCC_press || last_scancode == MAJ_R.SCC_press) && !MAJ_L.value) {
                MAJ_L.value = 1;
                continue;
            } else if ((last_scancode == MAJ_L.SCC_press || last_scancode == MAJ_R.SCC_press) && !MAJ_R.value) {
                MAJ_R.value = 1;
                continue;
            } else if ((last_scancode == MAJ_L.SCC_release || last_scancode == MAJ_R.SCC_release) && MAJ_L.value) {
                MAJ_L.value = 0;
                continue;
            } else if ((last_scancode == MAJ_L.SCC_release || last_scancode == MAJ_R.SCC_release) && MAJ_R.value) {
                MAJ_R.value = 0;
                continue;
            } else if (last_scancode == ALT_L.SCC_press && !ALT_L.value) {
                ALT_L.value = 1;
                continue;
            } else if (last_scancode == ALT_L.SCC_release && ALT_L.value) {
                ALT_L.value = 0;
                continue;
            } else if (last_scancode == CTRL.SCC_press && !CTRL.value) {
                CTRL.value = 1;

                if (last_scancode == etendue.SCC_press && !etendue.value)
                {
                    etendue.value = 1;
                }

                continue;
            } else if (last_scancode == CTRL.SCC_release && CTRL.value) {
                CTRL.value = 0;
                etendue.value = 0;
                continue;
            }

            if (CTRL.value && etendue.value) {
                CTRL_R = 1;
                CTRL_L = 0;
                continue;
            } else if (CTRL.value && !etendue.value) {
                CTRL_R = 0;
                CTRL_L = 1;
                continue;
            }

            if (CTRL_R && ALT_L.value){
                ALT_GR = 1;
            } else if (!CTRL_R || !ALT_L.value) {
                ALT_GR = 0;
            }
            
            if (last_scancode == 0x1C)
			{
                last_scancode = 0;
                cursor.position = ((cursor.position) / 160 + 1) * 160;

				if (0 == strcmp(memory, command.shutdown)) {
					// shutdown_function();
					break; // temporair exit
				} else if (0 == strcmp(memory, command.exit)) {
					break;
				} else if (0 == strcmp(memory, command.clear)) {
					clear();
					cursor.position = 0;
				} else if (0 == strcmp(memory, command.clist)) {
					print(command.clear);
					print("\n");
					print(command.clist);
					print("\n");
					print(command.exit);
					print("\n");
					print(command.shutdown);
					print("\n");
				} else {
					print("Error: Cette commande n'existe pas.\n");
				}

				erase_memory();
				nb_char = 0;
				print(PROMPT);
                continue;
            }
            
            if (last_scancode == 0x0E && cursor.position >= 2) // touche Delet
            {
                last_scancode = 0;
                cursor.position -= 2;
                video_memory[cursor.position] = ' ';
                video_memory[cursor.position+1] = 0x0F;

                nb_char--;
                memory[nb_char] = 0;
                continue;
            }

            if (cursor.position > 4000){
                for (int k = 160; k < 4000; k++)
                {
                    video_memory[k-160] = video_memory[k];
                }

                for (int k = 3840; k < 3999; k += 2)
                {
                    video_memory[k] = ' ';
                    video_memory[k+1] = 0x0F;
                }

                cursor.position = 3840;
            } else {
                uint8_t sc = last_scancode;
                last_scancode = 0;
                if (sc > 127)
                {
                    continue;
                }
                if (sc & 0x80)
                {
                    continue;
                }

                char c;
                // Essaie de mapper vers un caractère
                if ((MAJ_L.value || MAJ_R.value) && !ALT_GR) {
                    c = qwertz_german[1][sc];
                    memory[cursor.position] = c;
                } else if ((!MAJ_L.value && !MAJ_R.value) && ALT_GR) {
                    c = qwertz_german[2][sc];
                } else {
                    c = qwertz_german[0][sc];
                    memory[cursor.position] = c;
                }
				video_memory[cursor.position] = c;
				video_memory[cursor.position+1] = 0x0F;
                cursor.position += 2;
                memory[nb_char] = c;
                nb_char++;
            }
        }
    }
}

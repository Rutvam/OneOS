// Debut main.c
#include "./core/idt/IDT.h"
#include "./core/value/global_value.h"
#include "./core/function/math.h"
#include "./core/function/random.h"
#include "./core/function/memory.h"
#include "./core/function/function.h"
#include "./core/keyboard/keyboard.h"
#include "./pong/main_pong.h"
#include "./shell/main_shell.h"
#include "./graphics/graphics.h"
#include "./shutdown/main_shutdown.h"

// Une macro qui capture le fichier et la ligne automatiquement
#define ASSERT_OR_LOG(condition, message) \
    if (!(condition)) { \
        kernel_panic(message, __FILE__, __LINE__); \
    }
/*
void kernel_panic(const char* message, const char* file, int line)
{
    cursor.position = 0;
    // On efface l'écran ou on écrit directement
    // print("\n\033[31m========================================\033[0m\n");
    // print("\033[31m ! KERNEL PANIC ! \033[0m\n");
    // print("Message: %s\n", &cursor, message);
    // print("Fichier: %s\n", &cursor, file);
    			last_scancode = 0;
    // Note: Si ton 'set' ne gère pas le %d pour les entiers, 
    // tu devras convertir 'line' en chaîne de caractères d'abord.
    // print("Ligne  : %d\n", &cursor, line); 
    // print("\033[31m========================================\033[0m\n");

    // On arrête totalement le CPU pour plus que ça bouge
    __asm__ __volatile__("cli"); // Désactive les interruptions
    while(1) {
        __asm__ __volatile__("hlt"); // Met le CPU en veille prolongée
    }
}
*/

// Dans kernel.c
int main()
{
	// int nb_char = 0;
	char* video = (char*)0xB8000;
	print_main_menu();

	pic_remap();
	set_idt_gate(33, (uint32_t)(uintptr_t)keyboard_handler_asm);
	init_idt();

	__asm__ __volatile__("sti");
	// int ALT_GR = 0;
	// int CTRL_L = 0;
	// int CTRL_R = 0;
	uint8_t etendue = 0;
	pong.value = 0;
	shell.value = 0;
	shutdown.value = 1;
	while (1) {
		// print_main_menu();
		refrech_screen();
		save_screen();
        if (last_scancode) {
			uint8_t sc = last_scancode;
			last_scancode = 0;

			/*
			 * =========================
			 * TOUCHES ÉTENDUES E0
			 * =========================
			 */

			if (sc == 0xE0)
			{
				etendue = 1;
				continue;
			}

			if (etendue)
			{
    			etendue = 0;

				switch (sc)
				{
        			case 0x48: // UP
            			if (pong.value)
            			{
                			pong.value = 0;
                			shutdown.value = 1;
            			    shell.value = 0;
            			}
            			else if (shutdown.value)
            			{
            			    shutdown.value = 0;
            			    shell.value = 1;
            			    pong.value = 0;
            			}
            			else
            			{
                			shell.value = 0;
                			pong.value = 1;
                			shutdown.value = 0;
            			}
			            continue;


        			case 0x50: // DOWN
            			if (pong.value)
            			{
                			pong.value = 0;
            			    shell.value = 1;
            			    shutdown.value = 0;
            			}
            			else if (shell.value)
            			{
            			    shell.value = 0;
            			    shutdown.value = 1;
            			    pong.value = 0;
            			}
            			else
            			{
							shutdown.value = 0;
							pong.value = 1;
							shell.value = 0;
						}
            			continue;
    			}
    			continue;
			}

            
            if (sc == 0x1C)
			{
                if (pong.value && !shell.value && !shutdown.value)
                {
                	main_pong();
                	continue;
                }
                if (!pong.value && shell.value && !shutdown.value)
                {
                	main_shell();
                }
                if (!pong.value && !shell.value && shutdown.value)
                {
                	main_shutdown();
                }
                continue;
            }
        }
    }

    return 0;
}
// Fin main.c

#include "IDT.h"
#include "math.h"
#include "function.h"
// Une macro qui capture le fichier et la ligne automatiquement
#define ASSERT_OR_LOG(condition, message) \
    if (!(condition)) { \
        kernel_panic(message, __FILE__, __LINE__); \
    }


void kernel_panic(const char* message, const char* file, int line)
{
    // On efface l'écran ou on écrit directement
    print("\n\033[31m========================================\033[0m\n");
    print("\033[31m ! KERNEL PANIC ! \033[0m\n");
    print("Message: %s\n", message);
    print("Fichier: %s\n", file);
    // Note: Si ton 'set' ne gère pas le %d pour les entiers, 
    // tu devras convertir 'line' en chaîne de caractères d'abord.
    print("Ligne  : %d\n", line); 
    print("\033[31m========================================\033[0m\n");

    // On arrête totalement le CPU pour plus que ça bouge
    __asm__ __volatile__("cli"); // Désactive les interruptions
    while(1) {
        __asm__ __volatile__("hlt"); // Met le CPU en veille prolongée
    }
}

extern void keyboard_handler_asm();

void clear()
{
	char* video_memory = (char*) 0xB8000;
	video_memory[0] = ' ';
	for (int i = 2; i < 4000; i = i + 2)
	{
		video_memory[i] = ' ';
		video_memory[i-1] = 0x0F;
	}
}

// Cette fonction sera appelée à chaque fois que tu touches au clavier !
void keyboard_handler_c() {
    *(volatile char*)0xB8000 = 'X';
    // Étape 1: Lire le scancode pour vider le contrôleur clavier
    uint8_t scancode;
    __asm__ __volatile__("inb $0x60, %0" : "=a"(scancode));

	// Table de conversion Scancode Set 1 pour disposition QWERTZ Allemande (DE)
	unsigned char qwertz_german[130] = {
	   0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', 0xE1,   '`', '\b', // 0x00 - 0x0E (0xE1 = ß en CP437) 15
	'\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 0x81,  '+',  '\n',        // 0x0F - 0x1C (0x81 = ü en CP437) 14
	   0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',0x94, 0x84,  '^',              // 0x1D - 0x29 (0x94 = ö, 0x84 = ä) 13
	   0, '#', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.',  '-',    0,               // 0x2A - 0x36 13
	 '*',   0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,    0,    0,    0,        // 0x37 - 0x44
	   0,   0,   0,   0,   0,   0,   0, '7', '8', '9', '-',  '4',  '5',  '6', '+',    // 0x45 - 0x53
	 '1', '2', '3', '0', ',',   0,   0, '<',   0,   0                             // 0x54 - 0x5D 
	};
	// print("\033[32mTouche presser\033[0m");
	if (!(scancode & 0x80)) {
		// print("\033[32mLa touche n'est pas delet\n\033[0m");
        // On s'assure que le scancode ne dépasse pas la taille de notre table
		ASSERT_OR_LOG(scancode < 130, "\033[32mScancode de touche inconnu ou trop grand!\033[0m");
        if (scancode < 130) {
        	// print("\033[32mLa touche est plus petit que 130\n\033[0m");
            char touche = qwertz_german[scancode];

        	ASSERT_OR_LOG(touche != 0, "La variable `touche` a la valeur 0!");
            if (touche != 0) {
            	// print("\033[34m[Clavier] Une touche a ete pressee! [%c]\033[0m\n", touche);
            }
            if (touche == 'q') {              // par ex. appuyer sur 'q'
                __asm__ __volatile__("movb $0x00, %al\n" "movw $0xF4, %dx\n" "outb %al, %dx\n");
            }

        } else {
        	print("%c", scancode);
        }
    }

    // Étape 2: Affichage
    

    // Étape 3: Dire au PIC que c'est fini
    __asm__ __volatile__("outb %%al, %%dx" : : "a"(0x20), "d"(0x20));
}


{
	print(text);

}

// Dans kernel.c
int main()
{
    // 1. On prépare l'affichage
    clear();
    seed_random(7);
    int value = randint()%5;
    print("\033[38mThe Kernel\033[0m\n");
    print("\033[36mrand variable: %d\033[0m", value);

    // 2. Configuration matérielle (Une seule fois !)
    pic_remap();
    set_idt_gate(33, (uint32_t)(uintptr_t)keyboard_handler_asm);
    init_idt();
    
    print("IDT chargee avec succes.\n");

    // 3. On ouvre les vannes du clavier
    __asm__ __volatile__("sti");
    print("\033[32mInterruptions activees ! Appuie sur une touche...\033[0m\n");

    // 4. Boucle de repos (Le CPU attend sagement ici)
    while(1)
    {
        // On ne fait rien, le CPU tourne à vide et 
        // l'interruption clavier le réveillera automatiquement.
    }

    return 0;
}

// Debut main.c
#include "./core/idt/IDT.h"
#include "./core/value/global_value.h"
#include "./core/function/math.h"
#include "./core/function/function.h"
// Une macro qui capture le fichier et la ligne automatiquement
#define ASSERT_OR_LOG(condition, message) \
    if (!(condition)) { \
        kernel_panic(message, __FILE__, __LINE__); \
    }


void kernel_panic(const char* message, const char* file, int line)
{
    int cursor = 0;
    // On efface l'écran ou on écrit directement
    print("\n\033[31m========================================\033[0m\n");
    print("\033[31m ! KERNEL PANIC ! \033[0m\n");
    // print("Message: %s\n", &cursor, message);
    // print("Fichier: %s\n", &cursor, file);
    // Note: Si ton 'set' ne gère pas le %d pour les entiers, 
    // tu devras convertir 'line' en chaîne de caractères d'abord.
    // print("Ligne  : %d\n", &cursor, line); 
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

static const unsigned char qwertz_german[2][128] = {
    // Normal
    {
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0xE1, 0x27, '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 0x81, '+', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0x94, 0x84, '^',
        0, '<', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-',
        0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+',
        '1', '2', '3', '0', ',', 0, 0, '<', 0, 0
    },

    // Shift
    {
         0,     27,  '!',  '"',  0x15,  '$',  '%',  '&',  '/',  '(',  ')',   '=',   '?',   '`',   '\b',
        '\t',  'Q',  'W',  'E',  'R',   'T',  'Z',  'U',  'I',  'O',  'P',   0x9A,  '*',   '\n',
         0,    'A',  'S',  'D',  'F',   'G',  'H',  'J',  'K',  'L',  0x99,  0x8E,  0xF8,
         0,    '>',  'Y',  'X',  'C',   'V',  'B',  'N',  'M',  ';',  ':',   '_',
         0,    '*',   0,   ' ',   0,     0,    0,    0,    0,    0,    0,     0,     0,     0,
         0,     0,    0,    0,    0,     0,    0,   '/',  '(',  ')',  '_',   '4',   '5',   '6',   '+',
        '1',   '2',  '3',  '0',  ',',    0,    0,   '>',   0,    0
    }
};

volatile uint8_t last_scancode = 0;

void keyboard_handler_c() {
    uint8_t scancode;
    __asm__ __volatile__("inb $0x60, %0" : "=a"(scancode));
    last_scancode = scancode;
    __asm__ __volatile__("outb %%al, %%dx" : : "a"(0x20), "d"(0x20));
}


// Dans kernel.c
int main()
{
    char vBig[3] = "00";
    char vMid[3] = "00";
    char vLit[3] = "00";
    itoa(kernel_vBig, vBig);
    itoa(kernel_vMid, vMid);
    itoa(kernel_vLit, vLit);
    const char PROMPT[25] = {'T', 'h', 'e', ' ', 'K', 'e', 'r', 'n', 'e', 'l', ' ', 'v', vBig[0], vBig[1], '.', vMid[0], vMid[1], '.', vLit[0], vLit[1], ' ', '>', '>', ' ', 0x00};
    char* video_memory = (char*) 0xB8000;

    // 1. On prépare l'affichage
    clear();
    seed_random(7);
    int value = randint()%5;
    print(PROMPT);

    // 2. Configuration matérielle (Une seule fois !)
    pic_remap();
    set_idt_gate(33, (uint32_t)(uintptr_t)keyboard_handler_asm);
    init_idt();

    // 3. On ouvre les vannes du clavier
    __asm__ __volatile__("sti");
    int MAJ = 0;
    int CTRL_STRG = 0;
    // 4. Boucle de repos (Le CPU attend sagement ici)
    while (1)
    {
        if (last_scancode) {
            if (last_scancode == 0x2A || last_scancode == 0x36) {
                MAJ = 1;
            } else if (last_scancode == 0xAA || last_scancode == 0xB6) {
                MAJ = 0;
            }
            
            if (last_scancode == 0x1C)
            {
                uint8_t sc = last_scancode;
                last_scancode = 0;
                cursor.position = ((cursor.position) / 160 + 1) * 160;
                print(PROMPT);
                continue;
            }
            
            if (last_scancode == 0x0E && cursor.position >= 2) // touche Delet
            {
                uint8_t sc = last_scancode;
                last_scancode = 0;
                cursor.position -= 2;
                video_memory[cursor.position] = ' ';
                video_memory[cursor.position+1] = 0x0F;
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

                // Essaie de mapper vers un caractère
                char c = qwertz_german[MAJ][sc];
                video_memory[cursor.position] = c;
                video_memory[cursor.position+1] = 0x0F;
                cursor.position += 2;
            }
        }
        if (cursor.affiche) {
            cursor.temp_char = video_memory[cursor.position];
            cursor.temp_color = video_memory[cursor.position + 1];
            video_memory[cursor.position] = cursor.skin;
            video_memory[cursor.position + 1] = 0x0F;
        } else if (!cursor.affiche) {
            video_memory[cursor.position] = cursor.temp_char;
            video_memory[cursor.position + 1] = cursor.temp_color;
        }
        cursor.affiche = !cursor.affiche;
    }


    return 0;
}
// Fin main.c

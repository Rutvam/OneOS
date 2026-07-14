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

static inline void vga_set_cursor(int position)
{
    // Registre bas
    __asm__ __volatile__("outb %b0, %w1" : : "a"(0x0F), "d"(0x3D4));
    __asm__ __volatile__("outb %b0, %w1" : : "a"(position & 0xFF), "d"(0x3D5));

    // Registre haut
    __asm__ __volatile__("outb %b0, %w1" : : "a"(0x0E), "d"(0x3D4));
    __asm__ __volatile__("outb %b0, %w1" : : "a"((position >> 8) & 0xFF), "d"(0x3D5));
}

void putpixel(int x, int y, uint32_t color) {
    struct VBEInfo* vbe = (struct VBEInfo*) 0x9000;

    if (x < 0 || y < 0 || x >= vbe->XResolution || y >= vbe->YResolution)
        return;

    uint32_t* fb = (uint32_t*) vbe->PhysBasePtr;
    int pitch = vbe->pitch / 4;
    fb[y * pitch + x] = color;
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

static const unsigned char qwertz_german[3][128] = {
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
    },

    // ALT_GR
    {
        0, 27, 0xB9, 0xB2, 0xB3, 0xBC, 0xBD, '&', '{', '[', ']', '}', '\\', 0, 0,
        0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x9A, '~', 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x99, 0x8E, 0xF8,
        0, '|', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, '/', '(', ')', '_', '4', '5', '6', '+',
        '1', '2', '3', '0', ',', 0, 0, '>', 0, 0
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
    struct TOUCHE {
        int value;
        int SCC_press;
        int SCC_release;
    };

    struct VBEInfo* vbe = (struct VBEInfo*) 0x9000;
    uint32_t* fb = (uint32_t*) vbe->PhysBasePtr;

    putpixel(959, 540, 0x00000000); // Noire RGB
    putpixel(960, 540, 0x00FF5555); // Jaune RGB
    putpixel(961, 540, 0x00FF0000); // Rouge RGB

    int nb_char = 0;
    char memory[300] = {0x00};

    char vBig[3] = "00";
    char vMid[3] = "00";
    char vLit[3] = "00";
    itoa(kernel_vBig, vBig);
    itoa(kernel_vMid, vMid);
    itoa(kernel_vLit, vLit);
    if (vBig[1] != 0x00)
    {
        vBig[1] = vBig[0];
        vBig[0] = '0';
    }

    if (vMid[1] != 0x00)
    {
        vMid[1] = vMid[0];
        vMid[0] = '0';
    }

    if (vLit[1] != 0x00)
    {
        vLit[1] = vLit[0];
        vLit[0] = '0';
    }

    const char PROMPT[25] = {'K', 'e', 'r', 'n', 'e', 'l', ' ', 'v', vBig[0], vBig[1], '.', vMid[0], vMid[1], '.', vLit[0], vLit[1], ' ', ' ', ' ', '>', '>', ' ', 0x00};
    // char* video_memory = (char*) 0xB8000;

    // 1. On prépare l'affichage
    clear();
    seed_random(7);
    int value = randint()%5;
    //print(PROMPT);

    // 2. Configuration matérielle (Une seule fois !)
    pic_remap();
    set_idt_gate(33, (uint32_t)(uintptr_t)keyboard_handler_asm);
    init_idt();

    // 3. On ouvre les vannes du clavier
    __asm__ __volatile__("sti");
    struct TOUCHE MAJ_L = {
        .value = 0,
        .SCC_press = 0x2A,
        .SCC_release = 0xAA
    };
    struct TOUCHE MAJ_R = {
        .value = 0,
        .SCC_press = 0x36,
        .SCC_release = 0xB6
    };
    int ALT_GR = 0;
    struct TOUCHE CTRL = {
        .value = 0,
        .SCC_press = 0x1D,
        .SCC_release = 0x9D
    };
    int CTRL_L = 0;
    int CTRL_R = 0;
    struct TOUCHE etendue = {
        .value = 0,
        .SCC_press = 0x0E
    };
    struct TOUCHE ALT_L = {
        .value = 0,
        .SCC_press = 0x38,
        .SCC_release = 0xB8
    };
    // 4. Boucle de repos (Le CPU attend sagement ici)
    while (1)
    {
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
                uint8_t sc = last_scancode;
                last_scancode = 0;
                cursor.position = ((cursor.position) / 160 + 1) * 160;
                // print(PROMPT);
                continue;
            }
            
            if (last_scancode == 0x0E && cursor.position >= 2) // touche Delet
            {
                uint8_t sc = last_scancode;
                last_scancode = 0;
                cursor.position -= 2;
                // video_memory[cursor.position] = ' ';
                // video_memory[cursor.position+1] = 0x0F;
                continue;
            }

            if (cursor.position > 4000){
                for (int k = 160; k < 4000; k++)
                {
                    // video_memory[k-160] = video_memory[k];
                }

                for (int k = 3840; k < 3999; k += 2)
                {
                    // video_memory[k] = ' ';
                    // video_memory[k+1] = 0x0F;
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
                } else if ((!MAJ_L.value && !MAJ_R.value) && ALT_GR) {
                    c = qwertz_german[2][sc];
                } else {
                    c = qwertz_german[0][sc];
                }
                // video_memory[cursor.position] = c;
                // video_memory[cursor.position+1] = 0x0F;
                cursor.position += 2;
                memory[nb_char] = c;
                nb_char++;
            }
        }
        vga_set_cursor(cursor.position/2);
        // Mon Curseur
        /*
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
        */
    }


    return 0;
}
// Fin main.c

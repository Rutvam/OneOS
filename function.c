 #include <stdarg.h>
#include "function.h"
static unsigned long int next_graine = 1;

void seed_random(unsigned int seed)
{
	next_graine = seed;
}

int randint(void)
{
	next_graine *= 1103515245; 
	next_graine += 12345;
	return (unsigned int)(next_graine / 65536) % 32768;
}

// Une fonction utilitaire à mettre dans un de tes fichiers (ex: function.c)
void itoa(int num, char* str) {
    int i = 0;
    int isNegative = 0;

    // Gestion du chiffre 0
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Gestion des nombres négatifs
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    // On extrait chaque chiffre en partant de la fin (modulo 10)
    while (num != 0) {
        int rem = num % 10;
        str[i++] = rem + '0'; // C'est ici qu'on ajoute '0' pour l'ASCII !
        num = num / 10;
    }

    // Si le nombre était négatif, on ajoute le signe moins
    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Fin de chaîne

    // Comme on a extrait les chiffres à l'envers, il faut inverser la chaîne
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void print(const char* text, ...)
{
    char* video_memory = (char*) 0xB8000;
    int background_color = 0;
    int text_color = 15;
    int color = background_color * 16 + text_color;
    int j = 0;

	// 1. Déclaration d'une variable qui va pointer sur nos arguments secrets
    va_list argv;

    // 2. Initialisation : on dit à 'va_list' de démarrer juste après le paramètre 'text'
    va_start(argv, text);

    // [Ton code de traitement ici]


    for (int i = 0; i <= 4000 && text[j] != 0x00; )
    {
        // Si la case n'est pas vide, on passe à la ligne suivante
        if (video_memory[i] != ' ') {
            i = ((i / 160) + 1) * 160;
            continue; // On "continue" sans faire i += 2 pour ne pas sauter la première case !
        }

        // Détection d'une séquence ANSI Escape
        if (text[j] == '\033' && text[j+1] == '[') {
            // Cas du Reset : \033[0m
            if (text[j+2] == '0' && text[j+3] == 'm') {
                background_color = 0;
                text_color = 15;
                j += 4; // On avance de 4 caractères
                continue;
            }
            
            // Cas d'une couleur de texte simple : \033[3Xm (ex: \033[34m)
            if (text[j+2] == '3' && text[j+3] >= '1' && text[j+3] <= '8' && text[j+4] == 'm') {
                text_color = text[j+3] - '1'; // Attention aux correspondances de couleurs VGA !
                j += 5;
                continue;
            }

            // Cas d'une couleur de fond simple : \033[4Xm
            if (text[j+2] == '4' && text[j+3] >= '1' && text[j+3] <= '8' && text[j+4] == 'm') {
                background_color = text[j+3] - '1';
                j += 5;
                continue;
            }
            
            // Si c'est une séquence composite (ex: \033[34;42m), tu pourras l'ajouter ici.
            // En attendant, on saute la séquence pour ne pas crash
            j += 3; 
            continue;
        } 
        else if (text[j] == '\n') {
            i = ((i / 160) + 1) * 160;
            j++;
            continue;
        } 
        else if (text[j] == '\t') {
            i += 8; // Un tab vaut généralement 4 ou 8 espaces (ici 4 cases de 2 octets)
            j++;
            continue;
        } else if (text[j] == '%') {
            i++; // On passe au caractère suivant pour voir le type (%c, %s, etc.)

            if (text[j+1] == 'c') {
                // va_arg(args, TYPE) extrait le prochain argument.
                // Attention : en C, les 'char' passés en argument variable sont automatiquement convertis en 'int'.
                char c = (char) va_arg(argv, int);
                
                // Ici, tu affiches le caractère 'c' à l'écran
                char buffer[2] = {c, '\0'};
                print(buffer);
                continue;
            } else if (text[j+1] == 's') {
                // On extrait une chaîne de caractères (char*)
                char* s = va_arg(argv, char*);
                print(s);
                continue;
            }else if (text[j+1] == 'd') {
                int d = va_arg(argv, int);
                continue;
                
                // Un buffer de 12 caractères suffit amplement pour un "int" 32-bit 
                // (qui va de -2147483648 à 2147483647, soit 11 caractères + le '\0')
                char buffer[12]; 
                
                // On convertit le nombre en texte
                itoa(d, buffer);
                
                // On l'affiche avec ta fonction set
                print(buffer);
            }
        }
        

        // Écriture du caractère à l'écran
        color = background_color * 16 + text_color;
        video_memory[i] = text[j];
        video_memory[i + 1] = color;
        
        i += 2;
        j++;
        // 3. Nettoyage obligatoire à la fin
        }
    va_end(argv);
}

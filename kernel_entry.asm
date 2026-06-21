[bits 32]
section .text
global _start     ; Rend le point d'entrée visible pour ld !
[extern main]     ; Dit à NASM que la fonction 'main' est dans le fichier C

_start:
    ; --- CONFIGURATION DES REGISTRES 32 BITS ---
    ; 0x10 correspond à l'offset de ton segment de données dans la GDT
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; --- INITIALISATION DE LA PILE 32 BITS ---
    mov ebp, 0x90000
    mov esp, ebp

    ; --- LE GRAND SAUT VERS LE C ---
    call main     ; Saute dans ton code C (kernel.c)
    jmp $         ; Sécurité si le C s'arrête

; --- GESTIONNAIRE D'INTERRUPTION POUR LE CLAVIER ---
global keyboard_handler_asm
extern keyboard_handler_c ; On dit que la vraie logique sera écrite en C

keyboard_handler_asm:
    pusha                    ; Sauvegarde tous les registres du processeur (eax, ebx...)
    call keyboard_handler_c  ; Appelle notre fonction C
    popa                     ; Restaure les registres comme ils étaient avant
    iret                     ; Instruction spéciale pour quitter une interruption

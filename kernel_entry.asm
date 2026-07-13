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
    mov ebp, 0x8F000
    mov esp, 0x8E000 ; Laisser au moins 4KB pour la pile

    ; --- LE GRAND SAUT VERS LE C ---
    call main     ; Saute dans ton code C (kernel.c)
    jmp $         ; Sécurité si le C s'arrête

; --- GESTIONNAIRE D'INTERRUPTION POUR LE CLAVIER ---
global keyboard_handler_asm
extern keyboard_handler_c ; On dit que la vraie logique sera écrite en C

keyboard_handler_asm:
    push ds
    push es
    push fs
    push gs

    pusha
    cld

    mov ax, 0x10
    mov ds, ax
    mov es, ax

    call keyboard_handler_c

    popa

    pop gs
    pop fs
    pop es
    pop ds

    iret


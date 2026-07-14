; Debut boot.asm
[bits 16]          ; Mode réel 16 bits
[org 0x7c00]        ; Adresse de chargement du BIOS

start:
    ; Initialisation des segments de données et sauvegarde du disque
    mov [BOOT_DRIVE], dl
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov si, boot_v
    call BIOS_print
    call BIOS_newline
    mov si, kernel_v
    call BIOS_print
    call BIOS_newline

.boucle_principale:
    mov di, buffer      ; On remet le pointeur DI au début du buffer pour la nouvelle commande
    mov si, PROMPT
    call BIOS_print
    jmp BIOS_attente_clavier

; --- FONCTIONS ---

BIOS_print:
    .loop:
        lodsb              
        cmp al, 0          
        je .done           
        call BIOS_affichage
        jmp .loop
    .done:
        ret

BIOS_affichage:
    mov ah, 0x0E       
    int 0x10           
    ret

BIOS_newline:
    mov ah, 0x0E
    mov al, 0x0D       
    int 0x10
    mov al, 0x0A       
    int 0x10
    ret

BIOS_clear:
    mov ah, 0x00        
    mov al, 0x03        
    int 0x10            
    ret

strcmp:
    .loop:
        mov al, [si]      ; Caractère attendu
        mov bl, [di]      ; Caractère tapé
        cmp al, bl        
        jne .different    
        cmp al, 0         
        je .identique     
        inc si            
        inc di
        jmp .loop
    .different:
        cmp al, 1         ; Force ZF = 0
        ret
    .identique:
        ret

BIOS_attente_clavier:
    mov ah, 0x00      
    int 0x16            

    cmp al, 0x0D        
    je .touche_entree   

    cmp al, 0x08        
    je .touche_delet
    cmp al, 0x7F
    je .touche_delet
     
    stosb                
    call BIOS_affichage 
    jmp BIOS_attente_clavier 

    .touche_entree:
        mov byte [di], 0    
        call BIOS_newline   

        ; --- ANALYSE DE LA COMMANDE ---
        
        ; Test 1 : clear
        mov si, clear_commande
        mov di, buffer
        call strcmp         
        je .executer_clear  

        ; Test 2 : shutdown
        mov si, shutdown_commande
        mov di, buffer
        call strcmp         
        je .executer_shutdown

        ; Test 3 : kernel
        mov si, kernel_commande
        mov di, buffer
        call strcmp
        je .executer_kernel

        ; --- GESTION DE L'ERREUR ---
        mov si, erreur
        call BIOS_print
        call BIOS_newline
        jmp start.boucle_principale 

    .executer_clear:
        call BIOS_clear
        jmp start.boucle_principale

    .executer_kernel:
        mov ax, 0x4F02        ; Set VBE mode
        mov bx, MODE_ID | 0x4000  ; bit 14 = framebuffer linéaire
        int 0x10
        ; call activer_mode_graphique_1080p
        call sauvegarder_infos_framebuffer
        call BIOS_charger_kernel    ; 1. Charge le kernel depuis le disque vers la RAM (0x1000)
        jmp passer_en_mode_protege  ; 2. Bascule en 32 bits et saute sur le kernel !

    .executer_shutdown:
        mov ax, 0x5301
        xor bx, bx
        int 0x15
        mov ax, 0x530E
        xor bx, bx
        mov cx, 0x0102      
        int 0x15
        mov ax, 0x5307
        mov bx, 0x0001      
        mov cx, 0x0003      
        int 0x15
        mov dx, 0x0604
        mov ax, 0x2000
        out dx, ax
    .loop:
        hlt
        jmp .loop

    .touche_delet:
        cmp di, buffer      
        je BIOS_attente_clavier 
        dec di              
        mov si, delet
        call BIOS_print     
        jmp BIOS_attente_clavier

sauvegarder_infos_framebuffer
    mov di, 0x9000
    mov ax, 0x4F01
    mov cx, MODE_ID
    int 0x10
    ret

BIOS_charger_kernel:
    mov ah, 0x02        ; Fonction BIOS : Lire des secteurs
    mov al, 32          ; Lire 32 secteurs | 1 secteur = 512o | 32*512 = 16384o
    mov ch, 0           ; Cylindre 0
    mov dh, 0           ; Tête 0
    mov cl, 2           ; Secteur 2 (juste après le bootloader)
    mov dl, [BOOT_DRIVE]
    
    mov bx, 0
    mov es, bx
    mov bx, 0x1000      ; Destination en RAM : 0x1000
    
    int 0x13            
    jc .erreur          
    ret
    .erreur:
    	mov si, erreur_charge_kernel
    	call BIOS_print
    	call BIOS_newline
        jmp start.boucle_principale

; --- PASSAGE EN MODE PROTÉGÉ (32 bits) ---
passer_en_mode_protege:
    cli                     ; Désactiver les interruptions du BIOS
    lgdt [gdt_descriptor]   ; Charger la GDT
    mov eax, cr0
    or eax, 0x1             ; Activer le bit Protected Mode
    mov cr0, eax
    jmp 0x08:0x1000         ; Far Jump direct vers kernel_entry !

; --- DONNÉES GDT ALIGNÉES ---
align 4
gdt_start:
    dd 0x0, 0x0             ; Segment Nul obligatoire (8 octets)

gdt_code:
    dw 0xFFFF               ; Limite (bits 0-15) -> 2 octets
    dw 0x0                  ; Base (bits 0-15)   -> 2 octets
    db 0x0                  ; Base (bits 16-23)  -> 1 octet
    db 10011010b            ; Octet d'accès      -> 1 octet
    db 11001111b            ; Flags + Limite 16-19 -> 1 octet
    db 0x0                  ; Base (bits 24-31)  -> 1 octet
                            ; TOTAL = 8 octets !

gdt_data:
    dw 0xFFFF               ; Limite (bits 0-15) -> 2 octets
    dw 0x0                  ; Base (bits 0-15)   -> 2 octets
    db 0x0                  ; Base (bits 16-23)  -> 1 octet
    db 10010010b            ; Octet d'accès      -> 1 octet
    db 11001111b            ; Flags + Limite 16-19 -> 1 octet
    db 0x0                  ; Base (bits 24-31)  -> 1 octet
                            ; TOTAL = 8 octets !
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; --- DONNÉES TEXTE ET BUFFER ---
BOOT_DRIVE db 0         
PROMPT db ">> ", 0
erreur db "ERREUR: Cette commande n'existe pas", 0
erreur_charge_kernel db "ERREUR: Pendant le chargement du kernel", 0
delet db 0x08, 0x20, 0x08, 0
clear_commande db "clear", 0 
shutdown_commande db "shutdown", 0 
kernel_commande db "kernel", 0 
kernel_v db "3.2.12", 0
boot_v db "0.6.3", 0

; On place le buffer directement ici sans directive SECTION
buffer:
    times 64 db 0

; --- SIGNATURE DE COUPE ---
times 510-($-$$) db 0 
dw 0xaa55
; Fin boot.asm

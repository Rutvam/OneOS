#include <stdint.h>
#include "IDT.h"

// 1. On instancie officiellement nos variables globales ici (sans struct complète)
struct IDT_entry idt[256];
struct IDTR idtr;

// 2. Ta fonction pour configurer une case de l'IDT
void set_idt_gate(int n, uint32_t handler_address) {
    idt[n].offset_lowerbits = handler_address & 0xFFFF;
    idt[n].selector = 0x08; // Segment de code de ta GDT
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E; // Présent, Anneau 0, 32-bit Interrupt Gate
    idt[n].offset_higherbits = (handler_address >> 16) & 0xFFFF;
}

// 3. Ta fonction pour charger l'IDT dans le CPU
void init_idt() {
    idtr.limit = (sizeof(struct IDT_entry) * 256) - 1;
    idtr.base = (uint32_t)&idt;

    // L'instruction assembleur magique
    __asm__ __volatile__("lidt (%0)" : : "r" (&idtr));
}

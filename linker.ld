ENTRY(_start)

SECTIONS
{
    /* On dit au linker que notre code commence a l'adresse 0x1000 */
    . = 0x1000;

    /* On force le point d'entree _start en premier, puis le reste du code */
    .text : {
        *(.text._start)
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }

    /* On jette toutes les sections parasites de Linux qui gonflent le fichier */
    /DISCARD/ : {
        *(.note.gnu.property)
        *(.comment)
        *(.eh_frame)
    }
}

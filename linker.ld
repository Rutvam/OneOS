ENTRY(_start)

SECTIONS
{
    . = 0x100000;

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

    /DISCARD/ : {
        *(.note.gnu.property)
        *(.comment)
        *(.eh_frame)
    }
}

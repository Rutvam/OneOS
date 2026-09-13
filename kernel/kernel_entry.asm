[bits 64]
section .text
	global _start
	global keyboard_handler_asm
	global outb
	extern main
	_start:
		mov rsp, 0x8E000
		mov rbp, 0x8F000
		call main
		.hang:
			hlt
			jmp .hang

	keyboard_handler_asm:
		hlt
		jmp keyboard_handler_asm

	outb:
		; RDI = port
		; RSI = value
		mov dx, di
		mov al, sil
		out dx, al
		ret

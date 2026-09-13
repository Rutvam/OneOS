// Debut main.c
#include <efi.h>
#include <efilib.h>
#include <stdint.h>
#include "./core/idt/IDT.h"
#include "./function/function.h"
#include "./core/keyboard/keyboard.h"
#include "./data/global_value.h"
#include "./pong/main_pong.h"
#include "./shell/main_shell.h"
#include "./graphics/graphics.h"
#include "./shutdown/main_shutdown.h"

struct GPU_CONFIG {
	UINT64 FrameBuffer;
	UINT64 FrameBuffer_Size;
	UINT32 width;
	UINT32 height;
	UINT32 pitch;
};

struct LOADER_PARAMS {
	UINT32					UEFI_Version;
	UINT32					Bootloader_MajorVersion;
	UINT32					Bootloader_MinorVersion;

	UINT32					Memory_Map_Descriptor_Version;
	UINTN					Memory_Map_Descriptor_Size;
	EFI_MEMORY_DESCRIPTOR	*Memory_Map;
	UINTN					Memory_Map_Size;

	EFI_PHYSICAL_ADDRESS	Kernel_BaseAddress;
	UINTN					Kernel_Pages;

	EFI_RUNTIME_SERVICES	*RTServices;
	struct GPU_CONFIG		*GPU_Configs;
	EFI_FILE_INFO			*FileMeta;
	EFI_CONFIGURATION_TABLE	*ConfigTables;
	UINTN					Number_of_ConfigTables;

	EFI_TIME				BootTime;
};

// Dans kernel.c
int main(struct LOADER_PARAMS *params)
{
	var.gpu.FBSize = params->GPU_Configs->FrameBuffer_Size;
	my.init.function = init_function;
	my.init.function(1); // 1 = yes, it is with QEMU; 0 = No, it's not with QEMU
	my.init.var(params->GPU_Configs->FrameBuffer);
	my.clear();
	my.print.outb("[ OK ] Main\r\n");
	if (params == 0)
	{
		my.print.outb("[ X ] Params\r\n");
		while (1);
	}
	my.print.outb("[ OK ] Params\r\n");
	my.print.outb("FrameBuffer size: %d\r\n", var.gpu.FBSize);
	my.print.outb("UEFI version: %h\r\n", params->UEFI_Version);
	my.print.outb("Bootloader version: %d.%d\r\n", params->Bootloader_MajorVersion, params->Bootloader_MinorVersion);

	var.video[0] = '$';
	var.video[1] = 0xFF0000;
	var.video[2] = '"';
	var.video[3] = 0x00FF00;
	var.video[4] = '%';
	var.video[5] = 0x0000FF;
	var.video[6] = '&';
	var.video[7] = 0xFFFFFF;
	my.screen.analyse();

	while (1);

    return 0;
}
// Fin main.c

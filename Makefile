BUILD_KERNEL = ./compile/kernel
CFLAGS_KERNEL = -m64 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -c -O0
NASMFLAGS_KERNEL = -f elf64
LDFLAGS_KERNEL = -n -T
KERNEL_OBJ = $(BUILD_KERNEL)/main.o\
	$(BUILD_KERNEL)/function.o\
	$(BUILD_KERNEL)/math.o\
	$(BUILD_KERNEL)/memory.o\
	$(BUILD_KERNEL)/random.o\
	$(BUILD_KERNEL)/IDT.o \
	$(BUILD_KERNEL)/keyboard.o\
	$(BUILD_KERNEL)/global_value.o\
	$(BUILD_KERNEL)/graphics.o\
	$(BUILD_KERNEL)/pong/main_pong.o\
	$(BUILD_KERNEL)/shell/main_shell.o\
	$(BUILD_KERNEL)/shutdown/main_shutdown.o\
	$(BUILD_KERNEL)/kernel_entry.o


BUILD_BOOT = ./compile/boot
CFLAGS_BOOT = -ffreestanding -fno-stack-protector -fno-pie -c -O0 -I/usr/include/efi -I/usr/include/efi/x86_64 -fpic -fshort-wchar -mno-red-zone


compile-all: clean boot kernel

boot: ./boot/boot.c
	gcc $(CFLAGS_BOOT) ./boot/boot.c -o $(BUILD_BOOT)/boot.o
	ld -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic /usr/lib/crt0-efi-x86_64.o $(BUILD_BOOT)/boot.o -L/usr/lib -lefi -lgnuefi -o $(BUILD_BOOT)/boot.so
	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --output-target=efi-app-x86_64 --subsystem=10 $(BUILD_BOOT)/boot.so $(BUILD_BOOT)/boot.efi
	rm -rf $(BUILD_BOOT)/boot.o $(BUILD_BOOT)/boot.so

ranger: boot kernel
	sudo -v
	dd if=/dev/zero of=esp.img bs=1M count=64

	parted -s esp.img mklabel gpt
	parted -s esp.img mkpart ESP fat32 1MiB 100%
	parted -s esp.img set 1 esp on

	$(eval LODEV := $(shell sudo losetup -Pf --show esp.img))

	sudo mkfs.fat -F 32 $(LODEV)p1
	mkdir -p /tmp/my-esp
	sudo mount $(LODEV)p1 /tmp/my-esp
	sudo mkdir -p /tmp/my-esp/EFI/BOOT
	
	sudo cp $(BUILD_BOOT)/boot.efi /tmp/my-esp/EFI/BOOT/BOOTX64.EFI
	sudo cp $(BUILD_KERNEL)/kernel.elf /tmp/my-esp/kernel.elf

	sudo umount /tmp/my-esp

	sudo losetup -d $(LODEV)

# Kernel
kernel: ./main.c \
		./core/idt/IDT.c \
		./core/function/function.c

	mkdir -p $(BUILD_KERNEL)/pong
	mkdir -p $(BUILD_KERNEL)/shell
	mkdir -p $(BUILD_KERNEL)/shutdown

	gcc $(CFLAGS_KERNEL) ./main.c                    -o $(BUILD_KERNEL)/main.o
	gcc $(CFLAGS_KERNEL) ./core/function/function.c  -o $(BUILD_KERNEL)/function.o
	gcc $(CFLAGS_KERNEL) ./core/function/math.c      -o $(BUILD_KERNEL)/math.o
	gcc $(CFLAGS_KERNEL) ./core/function/memory.c    -o $(BUILD_KERNEL)/memory.o
	gcc $(CFLAGS_KERNEL) ./core/function/random.c    -o $(BUILD_KERNEL)/random.o
	gcc $(CFLAGS_KERNEL) ./core/idt/IDT.c            -o $(BUILD_KERNEL)/IDT.o
	gcc $(CFLAGS_KERNEL) ./core/keyboard/keyboard.c  -o $(BUILD_KERNEL)/keyboard.o
	gcc $(CFLAGS_KERNEL) ./core/value/global_value.c -o $(BUILD_KERNEL)/global_value.o
	gcc $(CFLAGS_KERNEL) ./graphics/graphics.c       -o $(BUILD_KERNEL)/graphics.o
	gcc $(CFLAGS_KERNEL) ./main.c                    -o $(BUILD_KERNEL)/main.o
	gcc $(CFLAGS_KERNEL) ./pong/main_pong.c          -o $(BUILD_KERNEL)/pong/main_pong.o
	gcc $(CFLAGS_KERNEL) ./shell/main_shell.c        -o $(BUILD_KERNEL)/shell/main_shell.o
	gcc $(CFLAGS_KERNEL) ./shutdown/main_shutdown.c  -o $(BUILD_KERNEL)/shutdown/main_shutdown.o

	nasm $(NASMFLAGS_KERNEL) ./kernel_entry.asm -o $(BUILD_KERNEL)/kernel_entry.o

	ld $(LDFLAGS_KERNEL) linker.ld -o $(BUILD_KERNEL)/kernel.elf $(KERNEL_OBJ)

	rm -rf $(KERNEL_OBJ)
	rmdir $(BUILD_KERNEL)/pong
	rmdir $(BUILD_KERNEL)/shell
	rmdir $(BUILD_KERNEL)/shutdown

VBox: ranger
	-VBoxManage unregistervm "MyOS" --delete
	rm -f "$(PWD)/esp.vdi"
	VBoxManage createvm --name "MyOS" --register
	VBoxManage modifyvm "MyOS" --memory 128 --firmware efi --boot1 disk
	VBoxManage storagectl "MyOS" --name "SATA Controller" --add sata --controller IntelAhci
	VBoxManage convertfromraw "$(PWD)/esp.img" "$(PWD)/esp.vdi" --format VDI
	VBoxManage storageattach "MyOS" --storagectl "SATA Controller" --port 0 --device 0 --type hdd --medium "$(PWD)/esp.vdi"
	VBoxManage startvm "MyOS"

QEMU: ranger
	cp /usr/share/edk2/x64/OVMF_VARS.4m.fd ./ovmf_vars.fd
	qemu-system-x86_64 \
		-drive format=raw,file=esp.img \
		-m 128M \
		-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd

QEMU-debug: ranger
	cp /usr/share/edk2/x64/OVMF_VARS.4m.fd ./ovmf_vars.fd
	qemu-system-x86_64 \
		-drive format=raw,file=esp.img \
		-m 128M \
		-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
		-S -s
# -S = CPU en pause; -s = QEMU ouvre GDB sur localhost:1234

clean:
	rm -rf $(BUILD_BOOT)
	rm -rf $(BUILD_KERNEL)
	mkdir -p $(BUILD_BOOT)
	mkdir -p $(BUILD_KERNEL)

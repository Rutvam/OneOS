# --- VARIABLES ---
NASMC = nasm
EMULATOR = qemu-system-x86_64
CC = gcc
LDC = ld

# Dossier de build
BUILD_DIR = ./COMPILE

# Flags de compilation
NASMFLAGS_BIN = -f bin
NASMFLAGS_ELF = -f elf32
CFLAGS        = -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -c

# NOUVEAU : On appelle officiellement le linker.ld !
LDFLAGS       = -m elf_i386 -T linker.ld --oformat binary

# Fichier image final
TARGET = $(BUILD_DIR)/os-image.bin

# --- RÈGLES ---

all: $(TARGET)
	$(EMULATOR) -drive format=raw,file=$(TARGET)

$(TARGET): boot.asm kernel_entry.asm kernel.c IDT.c
	@mkdir -p $(BUILD_DIR)
	
	# 1. Compile le Bootloader
	$(NASMC) $(NASMFLAGS_BIN) boot.asm -o $(BUILD_DIR)/boot.bin
	
	# 2. Compile le trampoline ASM
	$(NASMC) $(NASMFLAGS_ELF) kernel_entry.asm -o $(BUILD_DIR)/kernel_entry.o
	
	# 3. Compile le Kernel C et le module IDT C
	$(CC) $(CFLAGS) kernel.c -o $(BUILD_DIR)/kernel.o
	$(CC) $(CFLAGS) IDT.c -o $(BUILD_DIR)/IDT.o
	$(CC) $(CFLAGS) math.c -o $(BUILD_DIR)/math.o
	$(CC) $(CFLAGS) function.c -o $(BUILD_DIR)/function.o
	
	# 4. Fusionne TOUS les fichiers objets ensemble (On a ajouté IDT.o ici)
	$(LDC) $(LDFLAGS) $(BUILD_DIR)/kernel_entry.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/IDT.o $(BUILD_DIR)/math.o $(BUILD_DIR)/function.o -o $(BUILD_DIR)/kernel.bin

	# 5. On force la taille
	truncate -s 16384 $(BUILD_DIR)/kernel.bin
	
	# 6. On colle le tout
	cat $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin > $(TARGET)

clean:
	rm -rf $(BUILD_DIR)

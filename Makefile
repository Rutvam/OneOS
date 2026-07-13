# Fin Makefile
# --- VARIABLES ---
NASMC = nasm
EMULATOR = qemu-system-x86_64
CC = gcc
LDC = ld

# Dossier de build
BUILD_DIR = ./compile

# Flags de compilation
NASMFLAGS_BIN = -f bin
NASMFLAGS_ELF = -f elf32
CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -c

# Linker
LDFLAGS = -m elf_i386 -T linker.ld --oformat binary

# Fichier image final
TARGET = $(BUILD_DIR)/os-image.bin

# --- RÈGLES ---

# Règle par défaut → compile seulement
all: compile

# Compile tout le système
compile: $(TARGET)

# Lance QEMU avec l'image déjà compilée
test: $(TARGET)
	$(EMULATOR) -drive format=raw,file=$(TARGET)

test-TUI: $(TARGET)
	$(EMULATOR) -display curses -drive format=raw,file=$(TARGET)

# Construction de l'image
$(TARGET): ./boot.asm \
	       ./kernel_entry.asm \
	       ./main.c \
	       ./core/idt/IDT.c \
	       ./core/value/global_value.c \
	       ./core/function/math.c \
	       ./core/function/function.c

	@mkdir -p $(BUILD_DIR)

	$(NASMC) $(NASMFLAGS_BIN) boot.asm -o $(BUILD_DIR)/boot.bin
	$(NASMC) $(NASMFLAGS_ELF) kernel_entry.asm -o $(BUILD_DIR)/kernel_entry.o

	$(CC) $(CFLAGS) ./main.c -o $(BUILD_DIR)/main.o
	$(CC) $(CFLAGS) ./core/idt/IDT.c -o $(BUILD_DIR)/IDT.o
	$(CC) $(CFLAGS) ./core/value/global_value.c -o $(BUILD_DIR)/global_value.o
	$(CC) $(CFLAGS) ./core/function/math.c -o $(BUILD_DIR)/math.o
	$(CC) $(CFLAGS) ./core/function/function.c -o $(BUILD_DIR)/function.o

	$(LDC) $(LDFLAGS) \
		$(BUILD_DIR)/kernel_entry.o \
		$(BUILD_DIR)/main.o \
		$(BUILD_DIR)/IDT.o \
		$(BUILD_DIR)/global_value.o \
		$(BUILD_DIR)/math.o \
		$(BUILD_DIR)/function.o \
		-o $(BUILD_DIR)/kernel.bin

	truncate -s 16384 $(BUILD_DIR)/kernel.bin

	cat $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin > $(TARGET)

# Nettoyage
clean:
	rm -rf $(BUILD_DIR)
# Fin Makefile

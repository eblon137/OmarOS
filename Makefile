# Компиляторы и флаги
CXX = g++
AS = nasm
LD = ld

CXXFLAGS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wall -Wextra -O2
ASFLAGS = -f elf32
LDFLAGS = -melf_i386 -T boot/linker.ld

# Исходные файлы
BOOT_SRC = boot/boot.asm
KERNEL_SRC = kernel/kernel.cpp kernel/io.cpp kernel/terminal.cpp kernel/filesystem.cpp kernel/editor.cpp kernel/game.cpp kernel/chat.cpp

# Объектные файлы
BOOT_OBJ = $(BOOT_SRC:.asm=.o)
KERNEL_OBJ = $(KERNEL_SRC:.cpp=.o)

# Цель по умолчанию
all: myos.iso

# Компиляция ассемблерных файлов
%.o: %.asm
	@echo "Assembling $<..."
	@$(AS) $(ASFLAGS) $< -o $@

# Компиляция C++ файлов
%.o: %.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Линковка ядра
myos.bin: $(BOOT_OBJ) $(KERNEL_OBJ)
	@echo "Linking kernel..."
	@$(LD) $(LDFLAGS) -o $@ $^
	@echo "Kernel binary created: $@"

# Создание ISO образа
myos.iso: myos.bin
	@echo "Creating ISO image..."
	@mkdir -p iso/boot/grub
	@cp myos.bin iso/boot/
	@echo 'set timeout=0' > iso/boot/grub/grub.cfg
	@echo 'set default=0' >> iso/boot/grub/grub.cfg
	@echo 'menuentry "OmarOS" {' >> iso/boot/grub/grub.cfg
	@echo '  multiboot /boot/myos.bin' >> iso/boot/grub/grub.cfg
	@echo '  boot' >> iso/boot/grub/grub.cfg
	@echo '}' >> iso/boot/grub/grub.cfg
	@grub-mkrescue -o myos.iso iso
	@echo "ISO image created: myos.iso"

# Запуск в QEMU
run: myos.iso
	@echo "Running in QEMU..."
	@qemu-system-i386 -cdrom myos.iso -m 512M

# Запуск в QEMU с отладочной информацией
debug: myos.iso
	@echo "Running in QEMU with debug info..."
	@qemu-system-i386 -cdrom myos.iso -m 512M -monitor stdio -d int,cpu_reset -D qemu.log -no-reboot

# Очистка
clean:
	@echo "Cleaning up..."
	@rm -f $(BOOT_OBJ) $(KERNEL_OBJ) myos.bin myos.iso
	@rm -rf iso
	@echo "Clean complete."

.PHONY: all run debug clean
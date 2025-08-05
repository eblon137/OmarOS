#!/bin/bash
# Запуск ОС в QEMU

# Сборка проекта
make

# Запуск в QEMU
qemu-system-i386 -cdrom myos.iso
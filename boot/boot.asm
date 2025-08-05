; boot.asm - Multiboot-совместимый загрузчик
BITS 32
SECTION .multiboot
align 4

; Multiboot-заголовок
MULTIBOOT_PAGE_ALIGN    equ 1<<0
MULTIBOOT_MEMORY_INFO   equ 1<<1
MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

; Заголовок Multiboot
dd MULTIBOOT_HEADER_MAGIC
dd MULTIBOOT_HEADER_FLAGS
dd MULTIBOOT_CHECKSUM

SECTION .text
GLOBAL start
EXTERN kmain

start:
    ; Настройка стека
    mov esp, stack_top
    
    ; Сохраняем multiboot информацию
    push ebx    ; Указатель на структуру multiboot_info
    push eax    ; Магическое число Multiboot
    
    ; Вызов ядра на C++
    call kmain
    
    ; Если ядро вернется, зациклимся
    cli
.hang:
    hlt
    jmp .hang

SECTION .bss
align 16
stack_bottom:
    resb 16384 ; 16 КБ для стека
stack_top:

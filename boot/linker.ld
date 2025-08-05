/* linker.ld - Скрипт компоновщика для Multiboot */
ENTRY(start)

SECTIONS {
    /* Начинаем с 1 МБ - стандартное место для ядра */
    . = 1M;

    /* Сначала секция .text с кодом */
    .text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)
        *(.text)
    }

    /* Секция .rodata для констант */
    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }

    /* Секция .data для инициализированных данных */
    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
    }

    /* Секция .bss для неинициализированных данных */
    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }

    /* Отбрасываем информацию для отладки */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
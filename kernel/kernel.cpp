// kernel.cpp
#include "io.h"
#include "terminal.h"
#include "filesystem.h"
#include "editor.h"
#include "game.h"
#include "chat.h"

// Структура Multiboot
struct multiboot_info {
    unsigned long flags;
    unsigned long mem_lower;
    unsigned long mem_upper;
    unsigned long boot_device;
    unsigned long cmdline;
    unsigned long mods_count;
    unsigned long mods_addr;
    unsigned long syms[4];
    unsigned long mmap_length;
    unsigned long mmap_addr;
    unsigned long drives_length;
    unsigned long drives_addr;
    unsigned long config_table;
    unsigned long boot_loader_name;
    unsigned long apm_table;
    unsigned long vbe_control_info;
    unsigned long vbe_mode_info;
    unsigned short vbe_mode;
    unsigned short vbe_interface_seg;
    unsigned short vbe_interface_off;
    unsigned short vbe_interface_len;
    unsigned long framebuffer_addr;
    unsigned long framebuffer_pitch;
    unsigned long framebuffer_width;
    unsigned long framebuffer_height;
    unsigned char framebuffer_bpp;
    unsigned char framebuffer_type;
    unsigned char color_info[6];
};

// Структура для хранения аргументов команды
struct CommandArgs {
    int argc;           // Количество аргументов
    char* argv[16];     // Массив указателей на аргументы
    char buffer[256];   // Буфер для хранения строк аргументов
};

// Глобальные объекты
Terminal terminal;
FileSystem fs;
Editor editor(&terminal, &fs);
SnakeGame snakeGame(&terminal);
ChatBot chatBot(&terminal);

// Разбор строки команды на аргументы
void parseCommand(const char* cmd, CommandArgs* args) {
    args->argc = 0;
    
    // Копируем команду в буфер
    strcpy(args->buffer, cmd);
    
    // Разбиваем на аргументы
    char* token = args->buffer;
    char* end = args->buffer + strlen(args->buffer);
    
    while (token < end && args->argc < 16) {
        // Пропускаем пробелы
        while (*token == ' ' && token < end) {
            *token++ = '\0';
        }
        
        // Если достигли конца строки, выходим
        if (token >= end) break;
        
        // Сохраняем указатель на начало аргумента
        args->argv[args->argc++] = token;
        
        // Ищем конец аргумента
        while (*token != ' ' && *token != '\0' && token < end) {
            token++;
        }
    }
}

// Команда help - вывод справки
void cmdHelp() {
    unsigned char titleColor = terminal.makeColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    unsigned char cmdColor = terminal.makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    unsigned char descColor = terminal.makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    terminal.writeLineColored("Available commands:", titleColor);
    
    terminal.writeColored("  help", cmdColor);
    terminal.writeLineColored("     - Show this help", descColor);
    
    terminal.writeColored("  clear", cmdColor);
    terminal.writeLineColored("    - Clear the screen", descColor);
    
    terminal.writeColored("  ls", cmdColor);
    terminal.writeLineColored("       - List files in current directory", descColor);
    
    terminal.writeColored("  cd DIR", cmdColor);
    terminal.writeLineColored("   - Change to directory DIR", descColor);
    
    terminal.writeColored("  mkdir DIR", cmdColor);
    terminal.writeLineColored(" - Create a new directory", descColor);
    
    terminal.writeColored("  touch FILE", cmdColor);
    terminal.writeLineColored(" - Create a new empty file", descColor);
    
    terminal.writeColored("  rm PATH", cmdColor);
    terminal.writeLineColored("   - Remove a file or directory", descColor);
    
    terminal.writeColored("  cat FILE", cmdColor);
    terminal.writeLineColored("  - Display contents of a file", descColor);
    
    terminal.writeColored("  edit FILE", cmdColor);
    terminal.writeLineColored(" - Edit a file (simple text editor)", descColor);
    
    terminal.writeColored("  game", cmdColor);
    terminal.writeLineColored("     - Play Snake game", descColor);
    
    terminal.writeColored("  chat", cmdColor);
    terminal.writeLineColored("     - Chat with OmarOS bot", descColor);
    
    terminal.writeColored("  info", cmdColor);
    terminal.writeLineColored("     - Show system information", descColor);
    
    terminal.writeColored("  exit", cmdColor);
    terminal.writeLineColored("     - Shutdown the system", descColor);
}

// Команда info - вывод информации о системе
void cmdInfo(multiboot_info* mbi) {
    unsigned char titleColor = terminal.makeColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    unsigned char valueColor = terminal.makeColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    terminal.writeLineColored("System Information:", titleColor);
    
    terminal.writeColored("  OS Name: ", titleColor);
    terminal.writeLineColored("OmarOS v0.3", valueColor);
    
    // Информация о процессоре
    char vendor[13];
    unsigned int eax, ebx, ecx, edx;
    
    // Получаем ID производителя процессора
    asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));
    
    // Преобразуем регистры в строку
    *((unsigned int*)vendor) = ebx;
    *((unsigned int*)(vendor + 4)) = edx;
    *((unsigned int*)(vendor + 8)) = ecx;
    vendor[12] = '\0';
    
    terminal.writeColored("  CPU: ", titleColor);
    terminal.writeLineColored(vendor, valueColor);
    
    // Информация о памяти
    if (mbi->flags & 0x1) {
        char memStr[32];
        
        terminal.writeColored("  Lower Memory: ", titleColor);
        itoa(mbi->mem_lower, memStr, 10);
        terminal.writeColored(memStr, valueColor);
        terminal.writeLineColored(" KB", valueColor);
        
        terminal.writeColored("  Upper Memory: ", titleColor);
        itoa(mbi->mem_upper, memStr, 10);
        terminal.writeColored(memStr, valueColor);
        terminal.writeLineColored(" KB", valueColor);
    }
    
    // Информация о загрузчике
    if (mbi->flags & 0x200) {
        terminal.writeColored("  Boot Loader: ", titleColor);
        terminal.writeLineColored((const char*)mbi->boot_loader_name, valueColor);
    }
    
    terminal.writeColored("  File System: ", titleColor);
    terminal.writeLineColored("Virtual in-memory filesystem", valueColor);
    
    terminal.writeColored("  Features: ", titleColor);
    terminal.writeLineColored("Command history, colored output, file operations, games, chat", valueColor);
    
    terminal.writeColored("  Author: ", titleColor);
    terminal.writeLineColored("Omar", valueColor);
}

// Обработка команд
void processCommand(const char* cmd, multiboot_info* mbi) {
    // Если команда пустая, ничего не делаем
    if (strlen(cmd) == 0) {
        return;
    }
    
    // Разбираем команду на аргументы
    CommandArgs args;
    parseCommand(cmd, &args);
    
    // Если нет аргументов, выходим
    if (args.argc == 0) {
        return;
    }
    
    // Обработка команд
    if (strcmp(args.argv[0], "help") == 0) {
        cmdHelp();
    }
    else if (strcmp(args.argv[0], "clear") == 0) {
        terminal.clear();
    }
    else if (strcmp(args.argv[0], "ls") == 0) {
        fs.listDirectory();
    }
    else if (strcmp(args.argv[0], "cd") == 0) {
        if (args.argc > 1) {
            fs.changeDirectory(args.argv[1]);
        } else {
            terminal.writeLineColored("Usage: cd <directory>", terminal.makeColor(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
        }
    }
    else if (strcmp(args.argv[0], "mkdir") == 0) {
        if (args.argc > 1) {
            fs.createDirectory(args.argv[1]);
        } else {
            terminal.writeLineColored("Usage: mkdir <directory>", terminal.makeColor(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
        }
    }
    else if (strcmp(args.argv[0], "touch") == 0) {
        if (args.argc > 1) {
            fs.createFile(args.argv[1]);
        } else {
            terminal.writeLineColored("Usage: touch <filename>", terminal.makeColor(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
        }
    }
    else if (strcmp(args.argv[0], "rm") == 0) {
        if (args.argc > 1) {
            fs.remove(args.argv[1]);
        } else {
            terminal.writeLineColored("Usage: rm <path>", terminal.makeColor(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
        }
    }
    else if (strcmp(args.argv[0], "cat") == 0) {
        if (args.argc > 1) {
            fs.readFile(args.argv[1]);
        } else {
            terminal.writeLineColored("Usage: cat <filename>", terminal.makeColor(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
        }
    }
    else if (strcmp(args.argv[0], "edit") == 0) {
        if (args.argc > 1) {
            editor.edit(args.argv[1]);
        } else {
            terminal.writeLineColored("Usage: edit <filename>", terminal.makeColor(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
        }
    }
    else if (strcmp(args.argv[0], "game") == 0) {
        snakeGame.run();
    }
    else if (strcmp(args.argv[0], "chat") == 0) {
        chatBot.run();
    }
    else if (strcmp(args.argv[0], "info") == 0) {
        cmdInfo(mbi);
    }
    else if (strcmp(args.argv[0], "exit") == 0) {
        terminal.writeLineColored("System shutdown not implemented.", terminal.makeColor(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
        terminal.writeLineColored("Use Ctrl+C in QEMU or reset your computer.", terminal.makeColor(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
    }
    else {
        terminal.writeColored("Unknown command: ", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal.writeLine(args.argv[0]);
        terminal.writeLineColored("Type 'help' for a list of commands.", terminal.makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    }
}
// Точка входа в ядро
extern "C" void kmain(unsigned long magic, unsigned long addr) {
    // Проверка, что загрузились через Multiboot
    if (magic != 0x2BADB002) {
        // Ошибка загрузки, но мы не можем вывести сообщение, так как терминал еще не инициализирован
        return;
    }
    
    // Получаем информацию от Multiboot
    multiboot_info* mbi = (multiboot_info*)addr;
    
    // Инициализация терминала
    terminal.initialize();
    
    // Приветственное сообщение
    unsigned char titleColor = terminal.makeColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    terminal.writeLineColored("OmarOS v0.3 - Booting...", titleColor);
    
    // Выводим информацию о памяти, если доступна
    if (mbi->flags & 0x1) {
        char memStr[32];
        
        terminal.writeColored("Lower memory: ", terminal.makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
        itoa(mbi->mem_lower, memStr, 10);
        terminal.writeColored(memStr, terminal.makeColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
        terminal.writeLineColored(" KB", terminal.makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
        
        terminal.writeColored("Upper memory: ", terminal.makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
        itoa(mbi->mem_upper, memStr, 10);
        terminal.writeColored(memStr, terminal.makeColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
        terminal.writeLineColored(" KB", terminal.makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    }
    
    // Выводим имя загрузчика, если доступно
    if (mbi->flags & 0x200) {
        terminal.writeColored("Boot loader: ", terminal.makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
        terminal.writeLineColored((const char*)mbi->boot_loader_name, terminal.makeColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    }
    
    terminal.writeLineColored("\nInitializing file system...", terminal.makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    
    // Инициализация файловой системы
    fs.initialize();
    
    terminal.writeLineColored("System initialized successfully!", terminal.makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal.writeLineColored("Type 'help' for a list of available commands.", terminal.makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    terminal.writeLine("");
    
    char cmdBuffer[256];
    
    // Основной цикл командной строки
    while (true) {
        // Вывод приглашения с цветом
        unsigned char promptColor = terminal.makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        unsigned char pathColor = terminal.makeColor(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
        
        terminal.writeColored("root@OmarOS:", promptColor);
        terminal.writeColored(fs.getCurrentPath(), pathColor);
        terminal.writeColored("$ ", promptColor);
        
        // Чтение команды с поддержкой истории
        terminal.readLine(cmdBuffer, sizeof(cmdBuffer));
        
        // Обработка команды
        processCommand(cmdBuffer, mbi);
    }
}
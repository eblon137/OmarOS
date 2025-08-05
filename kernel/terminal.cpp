// terminal.cpp
#include "terminal.h"
#include "io.h"
#include "filesystem.h"

// Инициализация терминала
void Terminal::initialize() {
    videoMemory = (unsigned short*)0xB8000;
    cursorX = 0;
    cursorY = 0;
    defaultColor = makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    currentColor = defaultColor;
    historyCount = 0;
    historyCurrent = -1;
    clear();
}

// Создание цветового атрибута
unsigned char Terminal::makeColor(VgaColor fg, VgaColor bg) {
    return fg | (bg << 4);
}

// Установка текущего цвета
void Terminal::setColor(unsigned char color) {
    currentColor = color;
}

// Сброс цвета к значению по умолчанию
void Terminal::resetColor() {
    currentColor = defaultColor;
}

// Очистка экрана
void Terminal::clear() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            videoMemory[index] = (defaultColor << 8) | ' ';
        }
    }
    cursorX = 0;
    cursorY = 0;
    updateCursor();
}

// Вывод строки
void Terminal::write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            cursorY++;
            cursorX = 0;
        } else {
            const int index = cursorY * VGA_WIDTH + cursorX;
            videoMemory[index] = (currentColor << 8) | str[i];
            cursorX++;
        }
        
        if (cursorX >= VGA_WIDTH) {
            cursorY++;
            cursorX = 0;
        }
        
        // Прокрутка при необходимости
        if (cursorY >= VGA_HEIGHT) {
            // Сдвигаем все строки вверх
            for (int y = 0; y < VGA_HEIGHT - 1; y++) {
                for (int x = 0; x < VGA_WIDTH; x++) {
                    const int toIndex = y * VGA_WIDTH + x;
                    const int fromIndex = (y + 1) * VGA_WIDTH + x;
                    videoMemory[toIndex] = videoMemory[fromIndex];
                }
            }
            
            // Очищаем последнюю строку
            for (int x = 0; x < VGA_WIDTH; x++) {
                const int index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
                videoMemory[index] = (defaultColor << 8) | ' ';
            }
            
            cursorY = VGA_HEIGHT - 1;
        }
    }
    updateCursor();
}

// Вывод строки с цветом
void Terminal::writeColored(const char* str, unsigned char color) {
    unsigned char oldColor = currentColor;
    setColor(color);
    write(str);
    setColor(oldColor);
}

// Вывод строки с переводом строки
void Terminal::writeLine(const char* str) {
    write(str);
    write("\n");
}

// Вывод строки с переводом строки и цветом
void Terminal::writeLineColored(const char* str, unsigned char color) {
    writeColored(str, color);
    write("\n");
}

// Добавление команды в историю
void Terminal::addToHistory(const char* cmd) {
    // Не добавляем пустые команды или дубликаты последней команды
    if (strlen(cmd) == 0 || (historyCount > 0 && strcmp(cmdHistory[historyCount-1], cmd) == 0)) {
        return;
    }
    
    // Если история заполнена, сдвигаем все элементы
    if (historyCount == CMD_HISTORY_SIZE) {
        for (int i = 0; i < CMD_HISTORY_SIZE - 1; i++) {
            strcpy(cmdHistory[i], cmdHistory[i+1]);
        }
        historyCount--;
    }
    
    // Добавляем новую команду
    strcpy(cmdHistory[historyCount], cmd);
    historyCount++;
    historyCurrent = historyCount;
}

// Получение предыдущей команды из истории
const char* Terminal::getPreviousCommand() {
    if (historyCount == 0 || historyCurrent <= 0) {
        return 0;
    }
    
    historyCurrent--;
    return cmdHistory[historyCurrent];
}

// Получение следующей команды из истории
const char* Terminal::getNextCommand() {
    if (historyCount == 0 || historyCurrent >= historyCount - 1) {
        historyCurrent = historyCount;
        return "";
    }
    
    historyCurrent++;
    return cmdHistory[historyCurrent];
}

// Удаление символа из буфера
void Terminal::backspace(char* buffer, int& position) {
    if (position > 0) {
        for (int i = position - 1; i < strlen(buffer); i++) {
            buffer[i] = buffer[i+1];
        }
        position--;
        
        if (cursorX > 0) {
            cursorX--;
        } else if (cursorY > 0) {
            cursorY--;
            cursorX = VGA_WIDTH - 1;
        }
        
        const int index = cursorY * VGA_WIDTH + cursorX;
        videoMemory[index] = (currentColor << 8) | ' ';
        updateCursor();
    }
}

// Вставка символа в буфер
void Terminal::insertChar(char* buffer, int& position, char c) {
    int len = strlen(buffer);
    if (len < 255) {
        for (int i = len; i >= position; i--) {
            buffer[i+1] = buffer[i];
        }
        buffer[position] = c;
        position++;
        
        // Выводим символ
        char str[2] = {c, '\0'};
        write(str);
    }
}

// Автодополнение
void Terminal::autoComplete(char* buffer, int& position) {
    // Сохраняем текущее слово для автодополнения
    char word[256] = {0};
    int wordStart = position;
    
    // Находим начало текущего слова
    while (wordStart > 0 && buffer[wordStart-1] != ' ') {
        wordStart--;
    }
    
    // Копируем текущее слово
    int wordLen = position - wordStart;
    strncpy(word, buffer + wordStart, wordLen);
    word[wordLen] = '\0';
    
    // Если слово пустое, ничего не делаем
    if (wordLen == 0) return;
    
    // Получаем список файлов и команд для автодополнения
    const char* commands[] = {"help", "clear", "ls", "cd", "mkdir", "touch", "rm", "cat", "edit", "info", "exit", "game", "chat"};
    int numCommands = sizeof(commands) / sizeof(commands[0]);
    
    // Проверяем команды
    char matches[16][32];
    int matchCount = 0;
    
    // Сначала проверяем команды
    for (int i = 0; i < numCommands && matchCount < 16; i++) {
        if (strncmp(word, commands[i], wordLen) == 0) {
            strcpy(matches[matchCount++], commands[i]);
        }
    }
    
    // Затем проверяем файлы и директории
    extern FileSystem fs;
    fs.findMatches(word, wordLen, matches, matchCount, 16);
    
    // Если есть только одно совпадение, автоматически дополняем
    if (matchCount == 1) {
        // Удаляем текущее слово
        for (int i = 0; i < wordLen; i++) {
            backspace(buffer, position);
        }
        
        // Вставляем новое слово
        const char* match = matches[0];
        int matchLen = strlen(match);
        for (int i = 0; i < matchLen; i++) {
            insertChar(buffer, position, match[i]);
        }
    }
    // Если есть несколько совпадений, показываем их
    else if (matchCount > 1) {
        writeLine("");
        for (int i = 0; i < matchCount; i++) {
            write(matches[i]);
            write("  ");
        }
        writeLine("");
        
        // Выводим приглашение и текущую строку заново
        extern FileSystem fs;
        writeColored("root@OmarOS:", makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        writeColored(fs.getCurrentPath(), makeColor(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
        writeColored("$ ", makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        write(buffer);
    }
}

// Чтение строки с клавиатуры с поддержкой истории команд
void Terminal::readLine(char* buffer, int maxSize) {
    int i = 0;
    buffer[0] = '\0';
    
    while (true) {
        // Ждем нажатия клавиши
        while ((inb(0x64) & 1) == 0);
        
        // Читаем скан-код
        unsigned char scancode = inb(0x60);
        
        // Enter (конец ввода)
        if (scancode == 0x1C) {
            buffer[i] = '\0';
            writeLine("");
            addToHistory(buffer);
            return;
        }
        // Backspace
        else if (scancode == 0x0E && i > 0) {
            i--;
            buffer[i] = '\0';
            
            if (cursorX > 0) {
                cursorX--;
            } else if (cursorY > 0) {
                cursorY--;
                cursorX = VGA_WIDTH - 1;
            }
            
            const int index = cursorY * VGA_WIDTH + cursorX;
            videoMemory[index] = (currentColor << 8) | ' ';
            updateCursor();
        }
        // Стрелка вверх (предыдущая команда)
        else if (scancode == 0x48) {
            const char* prevCmd = getPreviousCommand();
            if (prevCmd) {
                // Очищаем текущую строку
                while (i > 0) {
                    i--;
                    if (cursorX > 0) {
                        cursorX--;
                    } else if (cursorY > 0) {
                        cursorY--;
                        cursorX = VGA_WIDTH - 1;
                    }
                    
                    const int index = cursorY * VGA_WIDTH + cursorX;
                    videoMemory[index] = (currentColor << 8) | ' ';
                }
                
                // Выводим предыдущую команду
                strcpy(buffer, prevCmd);
                i = strlen(buffer);
                write(buffer);
            }
        }
        // Стрелка вниз (следующая команда)
        else if (scancode == 0x50) {
            const char* nextCmd = getNextCommand();
            
            // Очищаем текущую строку
            while (i > 0) {
                i--;
                if (cursorX > 0) {
                    cursorX--;
                } else if (cursorY > 0) {
                    cursorY--;
                    cursorX = VGA_WIDTH - 1;
                }
                
                const int index = cursorY * VGA_WIDTH + cursorX;
                videoMemory[index] = (currentColor << 8) | ' ';
            }
            
            // Выводим следующую команду
            if (nextCmd) {
                strcpy(buffer, nextCmd);
                i = strlen(buffer);
                write(buffer);
            }
        }
        // Tab (автодополнение)
        else if (scancode == 0x0F) {
            autoComplete(buffer, i);
        }
        // Обычный символ
        else if (scancode < 0x80) {
            // Преобразование скан-кода в ASCII (упрощенно)
            static const char scanToASCII[] = {
                0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
                0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,
                0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
                0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
                '*', 0, ' '
            };
            
            if (scancode < sizeof(scanToASCII) && scanToASCII[scancode] && i < maxSize - 1) {
                char c = scanToASCII[scancode];
                buffer[i++] = c;
                buffer[i] = '\0';
                
                // Выводим символ
                char str[2] = {c, '\0'};
                write(str);
            }
        }
    }
}

// Обновление позиции курсора
void Terminal::updateCursor() {
    unsigned short position = cursorY * VGA_WIDTH + cursorX;
    
    // Младший байт
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position & 0xFF));
    
    // Старший байт
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

// Установка курсора в указанную позицию
void Terminal::setCursor(int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        cursorX = x;
        cursorY = y;
        updateCursor();
    }
}

// Вывод одного символа
void Terminal::writeChar(char c) {
    if (c == '\n') {
        cursorY++;
        cursorX = 0;
    } else {
        const int index = cursorY * VGA_WIDTH + cursorX;
        videoMemory[index] = (currentColor << 8) | c;
        cursorX++;
    }
    
    if (cursorX >= VGA_WIDTH) {
        cursorY++;
        cursorX = 0;
    }
    
    // Прокрутка при необходимости
    if (cursorY >= VGA_HEIGHT) {
        // Сдвигаем все строки вверх
        for (int y = 0; y < VGA_HEIGHT - 1; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                const int toIndex = y * VGA_WIDTH + x;
                const int fromIndex = (y + 1) * VGA_WIDTH + x;
                videoMemory[toIndex] = videoMemory[fromIndex];
            }
        }
        
        // Очищаем последнюю строку
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
            videoMemory[index] = (defaultColor << 8) | ' ';
        }
        
        cursorY = VGA_HEIGHT - 1;
    }
    
    updateCursor();
}
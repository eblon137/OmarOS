// editor.cpp
#include "editor.h"
#include "terminal.h"
#include "filesystem.h"
#include "io.h"

// Конструктор
Editor::Editor(Terminal* term, FileSystem* filesystem) {
    terminal = term;
    fs = filesystem;
    lineCount = 0;
    cursorLine = 0;
    cursorPos = 0;
}

// Запуск редактора для файла
void Editor::edit(const char* file) {
    // Сохраняем имя файла
    strcpy(filename, file);
    
    // Очищаем буфер
    for (int i = 0; i < MAX_LINES; i++) {
        buffer[i][0] = '\0';
    }
    
    lineCount = 1;
    cursorLine = 0;
    cursorPos = 0;
    
    // Загружаем содержимое файла, если он существует
    int fileIndex = fs->findFile(file);
    if (fileIndex != -1) {
        // Разбиваем содержимое файла на строки
        const char* content = fs->getFileContent(fileIndex);
        int line = 0;
        int pos = 0;
        
        for (int i = 0; content[i] != '\0' && line < MAX_LINES; i++) {
            if (content[i] == '\n') {
                buffer[line][pos] = '\0';
                line++;
                pos = 0;
                if (line >= MAX_LINES) break;
            } else if (pos < MAX_LINE_LENGTH - 1) {
                buffer[line][pos++] = content[i];
                buffer[line][pos] = '\0';
            }
        }
        
        lineCount = line + 1;
    }
    
    // Очищаем экран и отображаем буфер
    terminal->clear();
    terminal->writeLineColored("Simple Text Editor - Press ESC to exit, F2 to save", 
                              terminal->makeColor(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
    
    displayBuffer();
    
    // Основной цикл редактора
    bool exitEditor = false;
    
    while (!exitEditor) {
        // Ждем нажатия клавиши
        while ((inb(0x64) & 1) == 0);
        
        // Читаем скан-код
        unsigned char scancode = inb(0x60);
        
        // ESC - выход из редактора
        if (scancode == 0x01) {
            exitEditor = true;
            break;
        }
        
        // Обрабатываем нажатие клавиши
        handleKeypress(scancode);
    }
    
    // Возвращаемся в обычный режим
    terminal->clear();
}

// Отображение буфера
void Editor::displayBuffer() {
    terminal->clear();
    
    // Отображаем заголовок
    terminal->writeLineColored("Simple Text Editor - Press ESC to exit, F2 to save", 
                              terminal->makeColor(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
    
    // Отображаем содержимое буфера
    for (int i = 0; i < lineCount; i++) {
        terminal->writeLine(buffer[i]);
    }
    
    // Отображаем статусную строку
    displayStatusLine();
    
    // Устанавливаем курсор
    terminal->setCursor(cursorPos, cursorLine + 2); // +2 для заголовка и пустой строки
}

// Отображение статусной строки
void Editor::displayStatusLine() {
    char statusLine[81];
    char lineStr[10], posStr[10];
    
    itoa(cursorLine + 1, lineStr, 10);
    itoa(cursorPos + 1, posStr, 10);
    
    // Формируем статусную строку
    strcpy(statusLine, "Line: ");
    strcat(statusLine, lineStr);
    strcat(statusLine, " Col: ");
    strcat(statusLine, posStr);
    strcat(statusLine, " | ");
    strcat(statusLine, filename);
    
    // Переходим в конец экрана
    terminal->setCursor(0, terminal->getHeight() - 1);
    
    // Очищаем строку
    for (int i = 0; i < terminal->getWidth(); i++) {
        terminal->writeChar(' ');
    }
    
    // Отображаем статусную строку
    terminal->setCursor(0, terminal->getHeight() - 1);
    terminal->writeColored(statusLine, terminal->makeColor(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY));
    
    // Возвращаем курсор на место
    terminal->setCursor(cursorPos, cursorLine + 2);
}

// Обработка нажатия клавиши
void Editor::handleKeypress(unsigned char scancode) {
    // F2 - сохранение файла
    if (scancode == 0x3C) {
        saveFile();
        displayBuffer();
        return;
    }
    
    // Стрелка вверх
    if (scancode == 0x48 && cursorLine > 0) {
        cursorLine--;
        if (cursorPos > strlen(buffer[cursorLine])) {
            cursorPos = strlen(buffer[cursorLine]);
        }
        displayBuffer();
        return;
    }
    
    // Стрелка вниз
    if (scancode == 0x50 && cursorLine < lineCount - 1) {
        cursorLine++;
        if (cursorPos > strlen(buffer[cursorLine])) {
            cursorPos = strlen(buffer[cursorLine]);
        }
        displayBuffer();
        return;
    }
    
    // Стрелка влево
    if (scancode == 0x4B && cursorPos > 0) {
        cursorPos--;
        displayBuffer();
        return;
    }
    
    // Стрелка вправо
    if (scancode == 0x4D && cursorPos < strlen(buffer[cursorLine])) {
        cursorPos++;
        displayBuffer();
        return;
    }
    
    // Enter - новая строка
    if (scancode == 0x1C) {
        // Проверяем, не превышен ли лимит строк
        if (lineCount >= MAX_LINES) {
            return;
        }
        
        // Сдвигаем все строки после текущей
        for (int i = lineCount; i > cursorLine + 1; i--) {
            strcpy(buffer[i], buffer[i-1]);
        }
        
        // Создаем новую строку
        buffer[cursorLine + 1][0] = '\0';
        
        // Если курсор не в конце строки, переносим часть текста на новую строку
        if (cursorPos < strlen(buffer[cursorLine])) {
            strcpy(buffer[cursorLine + 1], buffer[cursorLine] + cursorPos);
            buffer[cursorLine][cursorPos] = '\0';
        }
        
        lineCount++;
        cursorLine++;
        cursorPos = 0;
        
        displayBuffer();
        return;
    }
    
    // Backspace - удаление символа слева от курсора
    if (scancode == 0x0E) {
        if (cursorPos > 0) {
            // Удаляем символ и сдвигаем текст
            for (int i = cursorPos - 1; i < strlen(buffer[cursorLine]); i++) {
                buffer[cursorLine][i] = buffer[cursorLine][i+1];
            }
            cursorPos--;
        } else if (cursorLine > 0) {
            // Если курсор в начале строки, объединяем с предыдущей строкой
            cursorPos = strlen(buffer[cursorLine - 1]);
            strcat(buffer[cursorLine - 1], buffer[cursorLine]);
            
            // Сдвигаем все строки
            for (int i = cursorLine; i < lineCount - 1; i++) {
                strcpy(buffer[i], buffer[i+1]);
            }
            
            lineCount--;
            cursorLine--;
        }
        
        displayBuffer();
        return;
    }
    
    // Delete - удаление символа справа от курсора
    if (scancode == 0x53) {
        if (cursorPos < strlen(buffer[cursorLine])) {
            // Удаляем символ и сдвигаем текст
            for (int i = cursorPos; i < strlen(buffer[cursorLine]); i++) {
                buffer[cursorLine][i] = buffer[cursorLine][i+1];
            }
        } else if (cursorLine < lineCount - 1) {
            // Если курсор в конце строки, объединяем со следующей строкой
            strcat(buffer[cursorLine], buffer[cursorLine + 1]);
            
            // Сдвигаем все строки
            for (int i = cursorLine + 1; i < lineCount - 1; i++) {
                strcpy(buffer[i], buffer[i+1]);
            }
            
            lineCount--;
        }
        
        displayBuffer();
        return;
    }
    
    // Обычный символ
    static const char scanToASCII[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
        0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        '*', 0, ' '
    };
    
    if (scancode < sizeof(scanToASCII) && scanToASCII[scancode]) {
        insertChar(scanToASCII[scancode]);
        displayBuffer();
    }
}

// Вставка символа в текущую позицию
void Editor::insertChar(char c) {
    // Проверяем, не превышен ли лимит длины строки
    if (strlen(buffer[cursorLine]) >= MAX_LINE_LENGTH - 1) {
        return;
    }
    
    // Сдвигаем текст вправо
    for (int i = strlen(buffer[cursorLine]) + 1; i > cursorPos; i--) {
        buffer[cursorLine][i] = buffer[cursorLine][i-1];
    }
    
    // Вставляем символ
    buffer[cursorLine][cursorPos] = c;
    cursorPos++;
}

// Удаление текущей строки
void Editor::deleteLine() {
    // Проверяем, не последняя ли это строка
    if (lineCount <= 1) {
        buffer[0][0] = '\0';
        cursorPos = 0;
        return;
    }
    
    // Сдвигаем все строки
    for (int i = cursorLine; i < lineCount - 1; i++) {
        strcpy(buffer[i], buffer[i+1]);
    }
    
    lineCount--;
    
    // Если удалили последнюю строку, перемещаем курсор на предыдущую
    if (cursorLine >= lineCount) {
        cursorLine = lineCount - 1;
    }
    
    // Проверяем позицию курсора
    if (cursorPos > strlen(buffer[cursorLine])) {
        cursorPos = strlen(buffer[cursorLine]);
    }
}

// Сохранение файла
void Editor::saveFile() {
    // Собираем содержимое буфера в одну строку
    char content[MAX_LINES * MAX_LINE_LENGTH];
    content[0] = '\0';
    
    for (int i = 0; i < lineCount; i++) {
        strcat(content, buffer[i]);
        if (i < lineCount - 1) {
            strcat(content, "\n");
        }
    }
    
    // Сохраняем файл
    fs->writeFile(filename, content);
    
    // Отображаем сообщение о сохранении
    terminal->setCursor(0, terminal->getHeight() - 1);
    terminal->writeColored("File saved successfully!", terminal->makeColor(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREEN));
}
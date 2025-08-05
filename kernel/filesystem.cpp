// filesystem.cpp
#include "filesystem.h"
#include "io.h"
#include "terminal.h"

// Инициализация файловой системы
void FileSystem::initialize() {
    // Устанавливаем корневой каталог
    strcpy(currentPath, "/");
    
    // Создаем несколько тестовых файлов и каталогов
    fileCount = 6;
    
    strcpy(files[0].name, "bin");
    files[0].isDirectory = true;
    files[0].size = 0;
    files[0].isSystemFile = true;
    
    strcpy(files[1].name, "home");
    files[1].isDirectory = true;
    files[1].size = 0;
    files[1].isSystemFile = true;
    
    strcpy(files[2].name, "etc");
    files[2].isDirectory = true;
    files[2].size = 0;
    files[2].isSystemFile = true;
    
    strcpy(files[3].name, "readme.txt");
    files[3].isDirectory = false;
    strcpy(files[3].content, "Welcome to OmarOS!\n\nThis is a simple operating system built with C++ and Assembly.\nIt provides basic shell commands and file operations.\n\nType 'help' to see available commands.");
    files[3].size = strlen(files[3].content);
    files[3].isSystemFile = true;
    
    strcpy(files[4].name, "kernel.sys");
    files[4].isDirectory = false;
    strcpy(files[4].content, "This is a binary file and cannot be displayed properly.");
    files[4].size = strlen(files[4].content);
    files[4].isSystemFile = true;
    
    strcpy(files[5].name, "version");
    files[5].isDirectory = false;
    strcpy(files[5].content, "OmarOS v0.3\nBuild date: 2023-08-05\nAuthor: Omar");
    files[5].size = strlen(files[5].content);
    files[5].isSystemFile = true;
}

// Смена текущего каталога
void FileSystem::changeDirectory(const char* path) {
    // Переход в корневой каталог
    if (strcmp(path, "/") == 0) {
        strcpy(currentPath, "/");
        return;
    }
    
    // Переход на уровень выше
    if (strcmp(path, "..") == 0) {
        // Находим последний слеш
        int len = strlen(currentPath);
        if (len <= 1) return; // Уже в корне
        
        int lastSlash = len - 1;
        while (lastSlash > 0 && currentPath[lastSlash] != '/') {
            lastSlash--;
        }
        
        // Если это не корневой каталог, обрезаем путь
        if (lastSlash > 0) {
            currentPath[lastSlash] = '\0';
        } else {
            // Если это корневой каталог, оставляем только "/"
            currentPath[1] = '\0';
        }
        return;
    }
    
    // Проверяем, существует ли указанный каталог
    for (int i = 0; i < fileCount; i++) {
        if (strcmp(files[i].name, path) == 0 && files[i].isDirectory) {
            // Добавляем каталог к текущему пути
            int len = strlen(currentPath);
            
            // Если текущий путь не заканчивается на "/", добавляем его
            if (currentPath[len - 1] != '/') {
                currentPath[len] = '/';
                len++;
            }
            
            // Добавляем имя каталога
            strcpy(currentPath + len, path);
            return;
        }
    }
    
    // Каталог не найден
    terminal.writeColored("Directory not found: ", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal.writeLine(path);
}

// Вывод списка файлов в текущем каталоге
void FileSystem::listDirectory() {
    unsigned char dirColor = terminal.makeColor(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    unsigned char fileColor = terminal.makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    unsigned char sysFileColor = terminal.makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    unsigned char sizeColor = terminal.makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    
    for (int i = 0; i < fileCount; i++) {
        if (files[i].isDirectory) {
            terminal.writeColored("[DIR]  ", dirColor);
            if (files[i].isSystemFile) {
                terminal.writeColored(files[i].name, sysFileColor);
                terminal.writeColored(" (system)", sysFileColor);
            } else {
                terminal.writeColored(files[i].name, dirColor);
            }
            terminal.writeLine("");
        } else {
            terminal.writeColored("[FILE] ", fileColor);
            
            if (files[i].isSystemFile) {
                terminal.writeColored(files[i].name, sysFileColor);
                terminal.writeColored(" (system)", sysFileColor);
            } else {
                terminal.writeColored(files[i].name, fileColor);
            }
            
            // Выводим размер файла
            char sizeStr[16];
            itoa(files[i].size, sizeStr, 10);
            terminal.writeColored("  (", sizeColor);
            terminal.writeColored(sizeStr, sizeColor);
            terminal.writeColored(" bytes)", sizeColor);
            terminal.writeLine("");
        }
    }
}

// Получение текущего пути
const char* FileSystem::getCurrentPath() {
    return currentPath;
}

// Создание нового каталога
void FileSystem::createDirectory(const char* name) {
    // Проверяем, не превышен ли лимит файлов
    if (fileCount >= MAX_FILES) {
        terminal.writeLineColored("Error: Maximum number of files reached.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        return;
    }
    
    // Проверяем корректность имени
    if (!isValidFileName(name)) {
        terminal.writeLineColored("Error: Invalid directory name.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        return;
    }
    
    // Проверяем, не существует ли уже файл с таким именем
    if (findFile(name) != -1) {
        terminal.writeLineColored("Error: File or directory already exists.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        return;
    }
    
    // Создаем новый каталог
    strcpy(files[fileCount].name, name);
    files[fileCount].isDirectory = true;
    files[fileCount].size = 0;
    files[fileCount].isSystemFile = false;
    fileCount++;
    
    terminal.writeColored("Directory created: ", terminal.makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal.writeLine(name);
}

// Создание нового файла
void FileSystem::createFile(const char* name) {
    // Проверяем, не превышен ли лимит файлов
    if (fileCount >= MAX_FILES) {
        terminal.writeLineColored("Error: Maximum number of files reached.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        return;
    }
    
    // Проверяем корректность имени
    if (!isValidFileName(name)) {
        terminal.writeLineColored("Error: Invalid file name.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        return;
    }
    
    // Проверяем, не существует ли уже файл с таким именем
    if (findFile(name) != -1) {
        terminal.writeLineColored("Error: File or directory already exists.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        return;
    }
    
    // Создаем новый файл
    strcpy(files[fileCount].name, name);
    files[fileCount].isDirectory = false;
    files[fileCount].content[0] = '\0';
    files[fileCount].size = 0;
    files[fileCount].isSystemFile = false;
    fileCount++;
    
    terminal.writeColored("File created: ", terminal.makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal.writeLine(name);
}

// Удаление файла или каталога
void FileSystem::remove(const char* path) {
    // Находим файл
    int index = findFile(path);
    
    if (index == -1) {
        terminal.writeColored("Error: File or directory not found: ", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal.writeLine(path);
        return;
    }
    
    // Проверяем, не системный ли это файл
    if (files[index].isSystemFile) {
        terminal.writeColored("Error: Cannot remove system file: ", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal.writeLine(path);
        return;
    }
    // Удаляем файл, сдвигая все последующие
    for (int i = index; i < fileCount - 1; i++) {
        strcpy(files[i].name, files[i+1].name);
        files[i].isDirectory = files[i+1].isDirectory;
        strcpy(files[i].content, files[i+1].content);
        files[i].size = files[i+1].size;
        files[i].isSystemFile = files[i+1].isSystemFile;
    }
    
    fileCount--;
    
    terminal.writeColored("Removed: ", terminal.makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal.writeLine(path);
}

// Чтение содержимого файла
void FileSystem::readFile(const char* name) {
    // Находим файл
    int index = findFile(name);
    
    if (index == -1) {
        terminal.writeColored("Error: File not found: ", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal.writeLine(name);
        return;
    }
    
    // Проверяем, что это файл, а не каталог
    if (files[index].isDirectory) {
        terminal.writeColored("Error: ", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal.writeColored(name, terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal.writeLineColored(" is a directory.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        return;
    }
    
    // Выводим содержимое файла
    terminal.writeLineColored("--- File content ---", terminal.makeColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal.writeLine(files[index].content);
    terminal.writeLineColored("--- End of file ---", terminal.makeColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
}

// Запись в файл
void FileSystem::writeFile(const char* name, const char* content) {
    // Находим файл
    int index = findFile(name);
    
    if (index == -1) {
        // Если файл не существует, создаем его
        if (fileCount >= MAX_FILES) {
            terminal.writeLineColored("Error: Maximum number of files reached.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
            return;
        }
        
        if (!isValidFileName(name)) {
            terminal.writeLineColored("Error: Invalid file name.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
            return;
        }
        
        index = fileCount++;
        strcpy(files[index].name, name);
        files[index].isDirectory = false;
        files[index].isSystemFile = false;
    } else if (files[index].isDirectory) {
        terminal.writeColored("Error: ", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal.writeColored(name, terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal.writeLineColored(" is a directory.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        return;
    } else if (files[index].isSystemFile) {
        terminal.writeColored("Error: Cannot modify system file: ", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal.writeLine(name);
        return;
    }
    
    // Проверяем размер содержимого
    int contentSize = strlen(content);
    if (contentSize >= MAX_FILE_SIZE) {
        terminal.writeLineColored("Error: Content too large for file.", terminal.makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        return;
    }
    
    // Записываем содержимое
    strcpy(files[index].content, content);
    files[index].size = contentSize;
    
    terminal.writeColored("File updated: ", terminal.makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal.writeLine(name);
}

// Получение содержимого файла
const char* FileSystem::getFileContent(int fileIndex) {
    if (fileIndex < 0 || fileIndex >= fileCount || files[fileIndex].isDirectory) {
        return "";
    }
    
    return files[fileIndex].content;
}

// Поиск файла по имени
int FileSystem::findFile(const char* name) {
    for (int i = 0; i < fileCount; i++) {
        if (strcmp(files[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Проверка корректности имени файла
bool FileSystem::isValidFileName(const char* name) {
    // Проверяем длину имени
    int len = strlen(name);
    if (len == 0 || len > 31) {
        return false;
    }
    
    // Проверяем допустимые символы
    for (int i = 0; i < len; i++) {
        char c = name[i];
        if (!((c >= 'a' && c <= 'z') || 
              (c >= 'A' && c <= 'Z') || 
              (c >= '0' && c <= '9') || 
              c == '.' || c == '_' || c == '-')) {
            return false;
        }
    }
    
    return true;
}

// Поиск файлов и директорий для автодополнения
void FileSystem::findMatches(const char* prefix, int prefixLen, char matches[][32], int& matchCount, int maxMatches) {
    for (int i = 0; i < fileCount && matchCount < maxMatches; i++) {
        if (strncmp(prefix, files[i].name, prefixLen) == 0) {
            strcpy(matches[matchCount++], files[i].name);
        }
    }
}
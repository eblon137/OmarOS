// filesystem.h
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

class Terminal;
extern Terminal terminal;

class FileSystem {
private:
    static const int MAX_PATH_LENGTH = 256;
    static const int MAX_FILES = 64;
    static const int MAX_FILE_SIZE = 4096;
    static const int SYSTEM_FILES = 6; // Количество системных файлов, которые нельзя удалять
    
    struct File {
        char name[32];
        bool isDirectory;
        char content[MAX_FILE_SIZE];
        int size;
        bool isSystemFile;
    };
    
    char currentPath[MAX_PATH_LENGTH];
    File files[MAX_FILES];
    int fileCount;

public:
    void initialize();
    void changeDirectory(const char* path);
    void listDirectory();
    const char* getCurrentPath();
    
    // Методы для работы с файлами
    void createDirectory(const char* name);
    void createFile(const char* name);
    void remove(const char* path);
    void readFile(const char* name);
    void writeFile(const char* name, const char* content);
    const char* getFileContent(int fileIndex);
    
    // Вспомогательные методы
    int findFile(const char* name);
    bool isValidFileName(const char* name);
    
    // Метод для автодополнения
    void findMatches(const char* prefix, int prefixLen, char matches[][32], int& matchCount, int maxMatches);
};

#endif
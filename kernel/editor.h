// editor.h
#ifndef EDITOR_H
#define EDITOR_H

class FileSystem;
class Terminal;

class Editor {
private:
    static const int MAX_LINES = 100;
    static const int MAX_LINE_LENGTH = 80;
    
    char buffer[MAX_LINES][MAX_LINE_LENGTH];
    int lineCount;
    int cursorLine;
    int cursorPos;
    char filename[32];
    Terminal* terminal;
    FileSystem* fs;
    
    void displayBuffer();
    void displayStatusLine();
    void handleKeypress(unsigned char scancode);
    void insertChar(char c);
    void deleteLine();
    void saveFile();

public:
    Editor(Terminal* term, FileSystem* filesystem);
    void edit(const char* file);
};

#endif
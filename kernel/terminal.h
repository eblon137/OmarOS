// terminal.h
#ifndef TERMINAL_H
#define TERMINAL_H

// Константы для VGA текстового режима
enum VgaColor {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW = 14,
    VGA_COLOR_WHITE = 15,
};

class Terminal {
private:
    static const int VGA_WIDTH = 80;
    static const int VGA_HEIGHT = 25;
    static const int CMD_HISTORY_SIZE = 10;
    
    unsigned short* videoMemory;
    int cursorX;
    int cursorY;
    unsigned char defaultColor;
    unsigned char currentColor;
    
    // История команд
    char cmdHistory[CMD_HISTORY_SIZE][256];
    int historyCount;
    int historyCurrent;

public:
    void initialize();
    void clear();
    void write(const char* str);
    void writeLine(const char* str);
    void writeColored(const char* str, unsigned char color);
    void writeLineColored(const char* str, unsigned char color);
    void readLine(char* buffer, int maxSize);
    void updateCursor();
    
    // Методы для работы с цветом
    unsigned char makeColor(VgaColor fg, VgaColor bg);
    void setColor(unsigned char color);
    void resetColor();
    
    // Методы для истории команд
    void addToHistory(const char* cmd);
    const char* getPreviousCommand();
    const char* getNextCommand();
    
    // Методы для текстового редактора
    void setCursor(int x, int y);
    void writeChar(char c);
    int getWidth() { return VGA_WIDTH; }
    int getHeight() { return VGA_HEIGHT; }
    
    // Методы для автодополнения
    void autoComplete(char* buffer, int& position);
    void backspace(char* buffer, int& position);
    void insertChar(char* buffer, int& position, char c);
};

#endif
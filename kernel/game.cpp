// game.cpp
#include "game.h"
#include "terminal.h"
#include "io.h"

SnakeGame::SnakeGame(Terminal* term) {
    terminal = term;
    
    // Инициализация змейки
    length = 3;
    for (int i = 0; i < length; i++) {
        snakeX[i] = WIDTH / 2 - i;
        snakeY[i] = HEIGHT / 2;
    }
    
    dir = RIGHT;
    score = 0;
    
    // Инициализация поля
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            field[y][x] = ' ';
        }
        field[y][WIDTH] = '\0';
    }
    
    generateFood();
}

void SnakeGame::generateFood() {
    // Генерируем случайные координаты для еды
    // В простой реализации используем простой алгоритм
    static int seed = 0;
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    foodX = (seed % (WIDTH - 2)) + 1;
    
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    foodY = (seed % (HEIGHT - 2)) + 1;
}

void SnakeGame::drawField() {
    terminal->clear();
    
    // Очищаем поле
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            field[y][x] = ' ';
        }
    }
    
    // Рисуем границы
    for (int x = 0; x < WIDTH; x++) {
        field[0][x] = '#';
        field[HEIGHT-1][x] = '#';
    }
    
    for (int y = 0; y < HEIGHT; y++) {
        field[y][0] = '#';
        field[y][WIDTH-1] = '#';
    }
    
    // Рисуем змейку
    for (int i = 0; i < length; i++) {
        if (snakeX[i] >= 0 && snakeX[i] < WIDTH && snakeY[i] >= 0 && snakeY[i] < HEIGHT) {
            field[snakeY[i]][snakeX[i]] = (i == 0) ? 'O' : 'o';
        }
    }
    
    // Рисуем еду
    field[foodY][foodX] = '*';
    
    // Выводим поле
    for (int y = 0; y < HEIGHT; y++) {
        terminal->writeLine(field[y]);
    }
    
    // Выводим счет
    char scoreStr[32];
    itoa(score, scoreStr, 10);
    terminal->writeColored("Score: ", terminal->makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal->writeLine(scoreStr);
    terminal->writeLineColored("Use arrow keys to move, ESC to exit", terminal->makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

void SnakeGame::moveSnake() {
    // Сдвигаем тело змейки
    for (int i = length - 1; i > 0; i--) {
        snakeX[i] = snakeX[i-1];
        snakeY[i] = snakeY[i-1];
    }
    
    // Двигаем голову
    switch (dir) {
        case UP:    snakeY[0]--; break;
        case DOWN:  snakeY[0]++; break;
        case LEFT:  snakeX[0]--; break;
        case RIGHT: snakeX[0]++; break;
    }
    
    // Проверяем, съела ли змейка еду
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        length++;
        score += 10;
        generateFood();
    }
}

bool SnakeGame::checkCollision() {
    // Проверка столкновения со стенами
    if (snakeX[0] <= 0 || snakeX[0] >= WIDTH-1 || snakeY[0] <= 0 || snakeY[0] >= HEIGHT-1) {
        return true;
    }
    
    // Проверка столкновения с собственным телом
    for (int i = 1; i < length; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            return true;
        }
    }
    
    return false;
}

void SnakeGame::run() {
    bool gameOver = false;
    
    while (!gameOver) {
        drawField();
        
        // Задержка
        for (volatile int i = 0; i < 5000000; i++) {}
        
        // Проверяем нажатие клавиш
        if (inb(0x64) & 1) {
            unsigned char scancode = inb(0x60);
            
            // ESC - выход
            if (scancode == 0x01) {
                break;
            }
            
            // Стрелки - изменение направления
            if (scancode == 0x48 && dir != DOWN) dir = UP;       // Вверх
            if (scancode == 0x50 && dir != UP) dir = DOWN;       // Вниз
            if (scancode == 0x4B && dir != RIGHT) dir = LEFT;    // Влево
            if (scancode == 0x4D && dir != LEFT) dir = RIGHT;    // Вправо
        }
        
        moveSnake();
        
        if (checkCollision()) {
            gameOver = true;
        }
    }
    
    // Экран окончания игры
    terminal->clear();
    terminal->writeLineColored("Game Over!", terminal->makeColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    char scoreStr[32];
    itoa(score, scoreStr, 10);
    terminal->writeColored("Your score: ", terminal->makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal->writeLine(scoreStr);
    terminal->writeLineColored("Press any key to continue...", terminal->makeColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    
    // Ждем нажатия клавиши
    while ((inb(0x64) & 1) == 0) {}
    inb(0x60);
    
    terminal->clear();
}
// game.h
#ifndef GAME_H
#define GAME_H

class Terminal;

class SnakeGame {
private:
    static const int WIDTH = 40;
    static const int HEIGHT = 20;
    static const int MAX_LENGTH = 100;
    
    Terminal* terminal;
    
    // Координаты змейки
    int snakeX[MAX_LENGTH];
    int snakeY[MAX_LENGTH];
    int length;
    
    // Координаты еды
    int foodX;
    int foodY;
    
    // Направление движения
    enum Direction { UP, DOWN, LEFT, RIGHT } dir;
    
    // Счет
    int score;
    
    // Игровое поле
    char field[HEIGHT][WIDTH+1];
    
    void drawField();
    void moveSnake();
    void generateFood();
    bool checkCollision();
    
public:
    SnakeGame(Terminal* term);
    void run();
};

#endif
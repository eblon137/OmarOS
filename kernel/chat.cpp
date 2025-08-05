// chat.cpp
#include "chat.h"
#include "terminal.h"
#include "io.h"

ChatBot::ChatBot(Terminal* term) {
    terminal = term;
}

const char* ChatBot::getRandomResponse() {
    // Простой генератор случайных чисел
    static int seed = 0;
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return responses[seed % NUM_RESPONSES];
}

const char* ChatBot::getSmartResponse(const char* message) {
    // Проверяем ключевые слова
    if (strstr(message, "привет") || strstr(message, "здравствуй")) {
        return "Привет! Рад вас видеть в OmarOS!";
    }
    if (strstr(message, "как дела") || strstr(message, "как ты")) {
        return "У меня все отлично! Я работаю на полную мощность.";
    }
    if (strstr(message, "помощь") || strstr(message, "help")) {
        return "Я могу помочь вам с основными командами. Введите 'help' в консоли.";
    }
    if (strstr(message, "игра") || strstr(message, "game")) {
        return "В OmarOS есть игра 'Змейка'. Запустите ее командой 'game'.";
    }
    if (strstr(message, "файл") || strstr(message, "file")) {
        return "Вы можете создавать файлы командой 'touch' и редактировать их с помощью 'edit'.";
    }
    if (strstr(message, "Omar") || strstr(message, "омар")) {
        return "Omar - создатель этой замечательной операционной системы!";
    }
    if (strstr(message, "пока") || strstr(message, "до свидания")) {
        return "До свидания! Надеюсь, вам понравилось общаться со мной.";
    }
    
    // Если нет ключевых слов, возвращаем случайный ответ
    return getRandomResponse();
}

void ChatBot::run() {
    // Упрощенная версия для отладки
    terminal->clear();
    terminal->writeLineColored("=== OmarOS Chat Bot ===", terminal->makeColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal->writeLineColored("Привет! Я чат-бот OmarOS. Напишите что-нибудь или 'exit' для выхода.", terminal->makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal->writeLine("");
    
    char message[256];
    bool exitChat = false;
    
    while (!exitChat) {
        // Вывод приглашения
        terminal->writeColored("Вы: ", terminal->makeColor(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
        
        // Чтение сообщения
        terminal->readLine(message, sizeof(message));
        
        // Проверка на выход
        if (strcmp(message, "exit") == 0) {
            exitChat = true;
            break;
        }
        
        // Получение ответа
        const char* response = getSmartResponse(message);
        
        // Вывод ответа
        terminal->writeColored("Бот: ", terminal->makeColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        terminal->writeLine(response);
        terminal->writeLine("");
    }
    
    terminal->clear();
}
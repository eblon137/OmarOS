// chat.h
#ifndef CHAT_H
#define CHAT_H

class Terminal;

class ChatBot {
private:
    Terminal* terminal;
    
    // Простые ответы бота
    static const int NUM_RESPONSES = 10;
    const char* responses[NUM_RESPONSES] = {
        "Интересно, расскажите больше.",
        "Я не совсем понимаю, что вы имеете в виду.",
        "OmarOS - лучшая операционная система!",
        "Как я могу вам помочь?",
        "Это очень интересная тема.",
        "Я думаю, что это отличная идея.",
        "Может быть, стоит попробовать что-то другое?",
        "Я бы хотел узнать больше об этом.",
        "Это звучит здорово!",
        "Я согласен с вами."
    };
    
    // Генерация случайного ответа
    const char* getRandomResponse();
    
    // Более умные ответы на ключевые слова
    const char* getSmartResponse(const char* message);
    
public:
    ChatBot(Terminal* term);
    void run();
};

#endif
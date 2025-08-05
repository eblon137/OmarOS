// io.cpp
#include "io.h"

// Чтение байта из порта
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

// Запись байта в порт
void outb(unsigned short port, unsigned char data) {
    __asm__("outb %0, %1" : : "a" (data), "Nd" (port));
}

// Длина строки
int strlen(const char* str) {
    int len = 0;
    while (str[len])
        len++;
    return len;
}

// Сравнение строк
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Сравнение n символов строк
int strncmp(const char* s1, const char* s2, int n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0)
        return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Копирование строки
void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

// Копирование n символов строки
void strncpy(char* dest, const char* src, int n) {
    int i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    // Если скопировано меньше n символов, заполняем остаток нулями
    for (; i < n; i++) {
        dest[i] = '\0';
    }
}

// Конкатенация строк
void strcat(char* dest, const char* src) {
    while (*dest)
        dest++;
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

// Поиск подстроки в строке
char* strstr(const char* haystack, const char* needle) {
    int needleLen = strlen(needle);
    if (needleLen == 0) {
        return (char*)haystack;
    }
    
    while (*haystack) {
        if (strncmp(haystack, needle, needleLen) == 0) {
            return (char*)haystack;
        }
        haystack++;
    }
    
    return 0;
}

// Преобразование числа в строку
void itoa(int value, char* str, int base) {
    static const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    // Проверка поддерживаемых оснований
    if (base < 2 || base > 36) {
        *str = '\0';
        return;
    }
    
    char* ptr = str;
    char* low = str;
    
    // Если отрицательное число и основание 10
    if (value < 0 && base == 10) {
        *ptr++ = '-';
        low = ptr;
        value = -value;
    }
    
    // Обрабатываем случай, когда value = 0
    if (value == 0) {
        *ptr++ = '0';
    } else {
        // Преобразуем число в строку
        while (value) {
            *ptr++ = digits[value % base];
            value /= base;
        }
    }
    
    *ptr-- = '\0';
    
    // Переворачиваем строку
    while (low < ptr) {
        char temp = *low;
        *low++ = *ptr;
        *ptr-- = temp;
    }
}
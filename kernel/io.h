// io.h
#ifndef IO_H
#define IO_H

// Функции ввода-вывода портов
unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);

// Строковые функции
int strlen(const char* str);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, int n);
void strcpy(char* dest, const char* src);
void strcat(char* dest, const char* src);
void strncpy(char* dest, const char* src, int n);  // Добавьте эту строку
char* strstr(const char* haystack, const char* needle);  // Добавьте эту строку
void itoa(int value, char* str, int base);

#endif
#pragma once


#define va_list int

#define va_start(x, y) x = 0; va_star(x, y);
void va_star(int, char *);
void vsprintf(char *, char *, int);
void va_end(int);

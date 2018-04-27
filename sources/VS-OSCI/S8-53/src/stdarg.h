#pragma once


#define va_list int
void va_start(int &, char *);
void vsprintf(char *, char *, int &);
void va_end(int);


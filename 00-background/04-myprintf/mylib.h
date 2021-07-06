#ifndef __MYLIB_H__
#define __MYLIB_H__
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

int puts(const char *s);
char *gets(char *s);
int vsprintf(char *text, const char *fmt, va_list ap);
int sprintf(char *text, const char *fmt, ...);
int printf(const char *fmt, ...);
int sscanf(const char *text, const char *fmt, ...);

typedef unsigned int uint;
#define TMAX 8192

#endif

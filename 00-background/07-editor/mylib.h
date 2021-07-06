#ifndef __MYLIB_H__
#define __MYLIB_H__
#include <stdio.h>
#include <stdlib.h> 
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

char* strcpy(char *s, const char *t);
int strcmp(const char *p, const char *q);
size_t strlen(const char *s);
char* strchr(const char *s, int c);
void* memset(void *dst, int c, size_t n);
void* memmove(void *vdst, const void *vsrc, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
int strtoi(const char *s, char **end, int base);
long strtol(const char *s, char **end, int base);
int puts(const char *s);
char *gets(char *s);
int vsprintf(char *text, const char *fmt, va_list ap);
int sprintf(char *text, const char *fmt, ...);
int printf1(const char *fmt, ...);
int sscanf(const char *text, const char *fmt, ...);

#define TMAX 8192
#define printf printf1

#endif

#ifndef __IO_H__
#define __IO_H__

#include <stddef.h>
#include <uart.h>

#define io_putc uart_putc
#define io_getc uart_getc

ssize_t io_read(void *buf, size_t n, int (*_getc)(void));
ssize_t io_write(const void *buf, size_t n, int (*_putc)(int));

#endif

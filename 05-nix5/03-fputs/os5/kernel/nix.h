#ifndef __NIX_H__
#define __NIX_H__

#include <board.h>
#include <std.h>
#include <string.h>

#define UART_THR    (char*)(UART+0x00) // THR:transmitter holding register 傳送暫存器
#define UART_RHR    (char*)(UART+0x00) // THR:transmitter holding register 傳送暫存器
#define UART_LSR    (char*)(UART+0x05) // LSR:line status register 輸出狀態暫存器
#define UART_LSR_RX_READY 0x01         // input is waiting to be read from RHR
#define UART_LSR_EMPTY_MASK 0x40       // LSR Bit 6: 當 LSR 的第六位元為 1 時，代表傳送區為空的，可以傳了 (Transmitter empty; both the THR and LSR are empty)

#define CONSOLE 1

#define NDEV  32
// map major device number to device functions.
struct devsw {
  ssize_t (*read)(void *, size_t);
  ssize_t (*write)(const void *, size_t);
};

extern struct devsw devsw[NDEV];
extern struct ftable ftable;

void nix_init(void);
int nix_start(void);
int main();
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
ssize_t cdev_write(const void *buf, size_t n, int (*_putc)(int));
ssize_t cdev_read(void *buf, size_t n, int (*_getc)(void));

// uart
int uart_putc(int ch);
int uart_getc();

// console
ssize_t console_read(void *buf, size_t n);
ssize_t console_write(const void *buf, size_t n);

// ramdisk
void ramdisk_init();
ssize_t ramdisk_read(void *buf, size_t n);
ssize_t ramdisk_write(const void *buf, size_t n);
void ramdisk_close();

#endif

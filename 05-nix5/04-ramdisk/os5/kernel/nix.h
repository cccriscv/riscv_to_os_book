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
#define RAMDISK 2

#define O_RDONLY        00000000
#define O_CREAT         00000100
#define O_TRUNC         00001000

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

int open(const char *pathname, int flags);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int close(int fd);

ssize_t cdev_write(const void *buf, size_t n, int (*_putc)(int));
ssize_t cdev_read(void *buf, size_t n, int (*_getc)(void));

// uart
int uart_putc(int ch);
int uart_getc();

// console init
void console_init();

// ramdisk
void ramdisk_init();
int ramdisk_open(int flags);
int ramdisk_close();

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#endif

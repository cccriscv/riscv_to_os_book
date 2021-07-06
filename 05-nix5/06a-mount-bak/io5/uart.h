#ifndef __UART_H__
#define __UART_H__

#include <board.h>

#define UART_THR    (char*)(UART+0x00) // THR:transmitter holding register 傳送暫存器
#define UART_RHR    (char*)(UART+0x00) // THR:transmitter holding register 傳送暫存器
#define UART_LSR    (char*)(UART+0x05) // LSR:line status register 輸出狀態暫存器
#define UART_LSR_RX_READY 0x01         // input is waiting to be read from RHR
#define UART_LSR_EMPTY_MASK 0x40       // LSR Bit 6: 當 LSR 的第六位元為 1 時，代表傳送區為空的，可以傳了 (Transmitter empty; both the THR and LSR are empty)

int uart_putc(int ch);
int uart_getc();

#endif

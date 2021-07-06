#include "uart.h"

int uart_putc(int ch) { // 透過 UART 傳送字元 ch 給宿主機印出
	while ((*UART_LSR & UART_LSR_EMPTY_MASK) == 0); // 一直等到 UART LSR 傳送區為空，可以傳送了 
	*UART_THR = (char) ch; // 將字元 ch 發送出去
  return 1;
}

int uart_getc() {
	while ((*UART_LSR & UART_LSR_RX_READY) == 0); // 一直等到 UART RX_READY 有收到資料 
	return (int) (*UART_RHR); // 傳回收到的字元
}
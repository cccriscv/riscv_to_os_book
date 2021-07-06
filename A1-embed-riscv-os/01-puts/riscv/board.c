#define UART        0x10000000         // RISC-V 的 virt 虛擬機，其 UART 映射區起始位址為 0x10000000
#define UART_THR    (char*)(UART+0x00) // THR:transmitter holding register 傳送暫存器
#define UART_RHR    (char*)(UART+0x00) // THR:transmitter holding register 傳送暫存器
#define UART_LSR    (char*)(UART+0x05) // LSR:line status register 輸出狀態暫存器
#define UART_LSR_RX_READY 0x01         // input is waiting to be read from RHR
#define UART_LSR_EMPTY_MASK 0x40       // LSR Bit 6: 當 LSR 的第六位元為 1 時，代表傳送區為空的，可以傳了 (Transmitter empty; both the THR and LSR are empty)

void uart_putc(char ch) { // 透過 UART 傳送字元 ch 給宿主機印出
	while ((*UART_LSR & UART_LSR_EMPTY_MASK) == 0); // 一直等到 UART LSR 傳送區為空，可以傳送了 
	*UART_THR = ch; // 將字元 ch 發送出去
}

char uart_getc() {
	while ((*UART_LSR & UART_LSR_RX_READY) == 0); // 一直等到 UART RX_READY 有收到資料 
	return *UART_RHR; // 傳回收到的字元
}

void _putc(char ch) {
	uart_putc(ch);
}

char _getc() {
	uart_getc();
}

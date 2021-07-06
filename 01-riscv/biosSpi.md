# BIOS SPI

* https://github.com/riscv/opensbi/

## UART

* https://github.com/riscv/opensbi/blob/master/lib/utils/serial/sifive-uart.c

```cpp
static void sifive_uart_putc(char ch)
{
	while (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
		;

	set_reg(UART_REG_TXFIFO, ch);
}

static int sifive_uart_getc(void)
{
	u32 ret = get_reg(UART_REG_RXFIFO);
	if (!(ret & UART_RXFIFO_EMPTY))
		return ret & UART_RXFIFO_DATA;
	return -1;
}
```
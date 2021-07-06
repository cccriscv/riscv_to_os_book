# asmio

```
user@DESKTOP-96FRN6B MINGW64 /d/ccc/ccc109/sp/10-riscv/04-asmio (master)
$ ./hello.sh
hello.elf: ELF 64-bit LSB executable, UCB RISC-V, version 1 (SYSV), statically linked, not stripped
HHello.
ello.
QEMU: Terminated // 按下 ctrl-A-X 會跳出。
```

之所以會有兩次 hello, 應該是因為沒有檢查 txready

* FreedomStudio-2020-06-3-win64\SiFive\freedom-e-sdk-v20.05.00.02\freedom-metal\src\drivers\sifive_uart0.c

```cpp
int __metal_driver_sifive_uart0_txready(struct metal_uart *uart) {
    long control_base = __metal_driver_sifive_uart0_control_base(uart);

    return !!((UART_REGW(METAL_SIFIVE_UART0_TXDATA) & UART_TXFULL));
}
```

然後在 FreedomStudio-2020-06-3-win64\SiFive\freedom-e-sdk-v20.05.00.02\freedom-metal\src\drivers\sifive_uart0.c

```cpp
/* TXDATA Fields */
#define UART_TXEN (1 << 0)
#define UART_TXFULL (1 << 31)
```
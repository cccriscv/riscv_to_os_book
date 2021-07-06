# Step 7 -- UART (a universal asynchronous receiver-transmitter)

* https://github.com/d0iasm/rvemu-for-book/blob/master/step07/src/uart.rs

```rust
    fn store8(&mut self, addr: u64, value: u64) {
        let (uart, _cvar) = &*self.uart;
        let mut uart = uart.lock().expect("failed to get an UART object");
        match addr {
            UART_THR => {
                print!("{}", value as u8 as char); // 這裡印出了一個字元。
                io::stdout().flush().expect("failed to flush stdout");
            }
            _ => {
                uart[(addr - UART_BASE) as usize] = value as u8;
            }
        }
    }
```

使用另一個 thread 來處理 read 的事情

```rust
    pub fn new() -> Self {
        let uart = Arc::new((Mutex::new([0; UART_SIZE as usize]), Condvar::new()));
        let interrupting = Arc::new(AtomicBool::new(false));
        {
            let (uart, _cvar) = &*uart;
            let mut uart = uart.lock().expect("failed to get an UART object");
            // Transmitter hold register is empty.
            uart[(UART_LSR - UART_BASE) as usize] |= UART_LSR_TX;
        }

        let mut byte = [0; 1];
        let cloned_uart = uart.clone();
        let cloned_interrupting = interrupting.clone();
        let _uart_thread_for_read = thread::spawn(move || loop {
            match io::stdin().read(&mut byte) {
                Ok(_) => {
                    let (uart, cvar) = &*cloned_uart;
                    let mut uart = uart.lock().expect("failed to get an UART object");
                    // Wait for the thread to start up.
                    while (uart[(UART_LSR - UART_BASE) as usize] & UART_LSR_RX) == 1 {
                        uart = cvar.wait(uart).expect("the mutex is poisoned");
                    }
                    uart[0] = byte[0];
                    cloned_interrupting.store(true, Ordering::Release);
                    // Data has been receive.
                    uart[(UART_LSR - UART_BASE) as usize] |= UART_LSR_RX;
                }
                Err(e) => {
                    println!("{}", e);
                }
            }
        });
        Self { uart, interrupting }
```

## helloworld.c

* https://github.com/d0iasm/rvemu-for-book/blob/master/step07/helloworld.c

```c
int main() {
    volatile char *uart = (volatile char *) 0x10000000;
    uart[0] = 'H';
    uart[0] = 'e';
    uart[0] = 'l';
    uart[0] = 'l';
    uart[0] = 'o';
    uart[0] = ',';
    uart[0] = ' ';
    uart[0] = 'w';
    uart[0] = 'o';
    uart[0] = 'r';
    uart[0] = 'l';
    uart[0] = 'd';
    uart[0] = '!';
    uart[0] = '\n';
    return 0;
}
```

## echoback.c

* https://github.com/d0iasm/rvemu-for-book/blob/master/step07/echoback.c

```c
int main() {
    while (1) {
        volatile char *uart = (volatile char *) 0x10000000;
        while ((uart[5] & 0x01) == 0);
        char c = uart[0];
        if ('a' <= c && c <= 'z') {
            c = c + 'A' - 'a';
        }
        uart[0] = c;
    }
}
```
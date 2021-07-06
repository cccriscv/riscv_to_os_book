# mini_riscv

RISC-V 工具鏈，包含 

1. asm -- 組譯器
2. dasm -- 反組譯器
3. vm -- 虛擬機

## 版本

01-只有三個指令 (addi, add, blt) 的版本
02-大部分指令，但不支援 csr 類指令。
03-大部分指令，且支援 csr 類指令。
04-支援輸出入 (putc) (用記憶體映射的方式，從 IO_BASE 開始安排)
05-asm 支援 data 字串宣告
06-改 oAdd 為 enum ... Add
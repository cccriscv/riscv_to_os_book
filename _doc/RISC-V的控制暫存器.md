# RISC-V的控制暫存器 (CSR)

CSR 類的指令，通常是在設定控制位元：

file: cpu.cpp

```cpp
void exec(Op *o) {
  //...
  else if (o==&oCsrrw) { x[rd]=csrs[csr]; csrs[csr]=x[rs1]; }
  else if (o==&oCsrrs) { x[rd]=csrs[csr]; csrs[csr]|=x[rs1]; }
  else if (o==&oCsrrc) { x[rd]=csrs[csr]; csrs[csr]&=(~x[rs1]); }
  else if (o==&oCsrrwi) { x[rd]=csrs[csr]; csrs[csr]=imm; }
  else if (o==&oCsrrsi) { x[rd]=csrs[csr]; csrs[csr]|=imm; }
  else if (o==&oCsrrci) { x[rd]=csrs[csr]; csrs[csr]&=(~imm); }
```

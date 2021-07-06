* https://www.sifive.com/documentation
    * [SiFive FE310-G000 Manual](https://sifive.cdn.prismic.io/sifive/4faf3e34-4a42-4c2f-be9e-c77baa4928c7_fe310-g000-manual-v3p2.pdf)
    * [SiFive FE310-G000 Preliminary Datasheet](https://sifive.cdn.prismic.io/sifive/e4b1c37c-401b-40c3-8e22-6a3967566730_fe310-g000-datasheet-v1p6.pdf)
    * [E76 Core Complex Manual](https://sifive.cdn.prismic.io/sifive/fa68b7d0-366c-4c05-bb2a-64a21f3dfdb0_e76_core_complex_manual_21G1.pdf)
    * [SiFive HiFive1 Rev B Getting Started Guide](https://sifive.cdn.prismic.io/sifive/cf239fd0-ae4f-4fd8-a944-fdafb5018153_hifive1b-getting-started-guide_v1.2.pdf)

```
5.13 Boot Flow
This process is managed as part of the Freedom Metal source code. The freedom-metal boot code supports single core boot or multi-core boot, and contains all the necessary initialization code to enable every core in the system.
1. ENTRY POINT: File: freedom-metal/src/entry.S, label: _enter.
2. Initialize global pointer gp register using the generated symbol __global_pointer$.
3. Write mtvec register with early_trap_vector as default exception handler.
4. Clear feature disable CSR 0x7c1.
5. Read mhartid into register a0 and call _start, which exists in crt0.S.
6. We now transition to File: freedom-metal/gloss/crt0.S, label: _start.
7. Initialize stack pointer, sp, with _sp generated symbol. Harts with mhartid of one or larger are offset by (_sp + __stack_size × mhartid). The __stack_size field is generated in the linker file.


5.15 RISC‑V Compiler Flags
5.15.1 arch, abi, and mtune
RISC‑V targets are described using three arguments:


$ riscv64-unknown-elf-gcc test.c -march=rv64imafdc -mabi=lp64d -o- -S -O3
dmul:
fmul.d fa0,fa0,fa1
ret
but will compile to an emulation routine without the D extension:
$ riscv64-unknown-elf-gcc test.c -march=rv64i -mabi=lp64 -o- -S -O3

```
[SiFive U54 Core Complex Manual](https://sifive.cdn.prismic.io/sifive/ae8793df-a5f0-43ca-a425-b574a4330d8d_u54_core_complex_manual_21G1.pdf)

```
Term Definition
AES Advanced Encryption Standard
BHT Branch History Table
BTB Branch Target Buffer
CBC Cipher Block Chaining
CCM Counter with CBC-MAC
CFM Cipher FeedBack
CLIC Core-Local Interrupt Controller. Configures priorities and levels for corelocal interrupts.
CLINT Core-Local Interruptor. Generates per hart software interrupts and timer
interrupts.
CTR CounTeR mode
DTIM Data Tightly Integrated Memory
ECB Electronic Code Book
GCM Galois/Counter Mode
hart HARdware Thread
IJTP Indirect-Jump Target Predictor
ITIM Instruction Tightly Integrated Memory
JTAG Joint Test Action Group
LIM Loosely-Integrated Memory. Used to describe memory space delivered in
a SiFive Core Complex that is not tightly integrated to a CPU core.
MDP Memory Dependence Predictor
MSHR Miss Status Handling Register
NLP Next-Line Predictor
OFB Output FeedBack
PLIC Platform-Level Interrupt Controller. The global interrupt controller in a
RISC‑V system.
PMP Physical Memory Protection
RAS Return-Address Stack
RO Used to describe a Read-Only register field.
ROB Reorder Buffer
RW Used to describe a Read/Write register field.
RW1C Used to describe a Read/Write-1-to-Clear register field.
SHA Secure Hash Algorithm
TileLink A free and open interconnect standard originally developed at UC Berkeley.
TRNG True Random Number Generator
WARL Write-Any, Read-Legal field. A register field that can be written with any
value, but returns only supported values when read.
WIRI Writes-Ignored, Reads-Ignore field. A read-only register field reserved for
future use. Writes to the field are ignored, and reads should ignore the
value returned.
```

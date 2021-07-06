# RISCV指南

## 導讀文件

* [RISC-V 手册 - 一本开源指令集的指南(PDF)](http://crva.ict.ac.cn/documents/RISC-V-Reader-Chinese-v2p1.pdf)
* https://riscv.org/technical/specifications/
    * https://github.com/riscv/riscv-isa-manual/releases/download/draft-20201001-bec9104/riscv-privileged.pdf
    * https://github.com/riscv/riscv-isa-manual/releases/download/draft-20201001-bec9104/riscv-spec.pdf
* https://rcore-os.github.io/rCore_tutorial_doc/ (第二版) (重要關鍵)
    * https://rcore-os.github.io/rCore-Tutorial-deploy/ (第三版)
    * https://github.com/rcore-os/rCore_tutorial/
    * https://rcore-os.github.io/rCore_tutorial_doc/chapter2/part5.html
    * 使用 OpenSBI - 新版 Qemu 中内置了 OpenSBI firmware（固件），它主要负责在操作系统运行前的硬件初始化和加载操作系统的功能。我们使用以下命令尝试运行一下：
* [QEMU: ‘virt’ generic virtual platform (virt)](https://www.qemu.org/docs/master/system/arm/virt.html)
    * QEMU riscv virt 原始碼 -- https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c
    * The virt board is a platform which does not correspond to any real hardware; it is designed for use in virtual machines. It is the recommended board type if you simply want to run a guest such as Linux and do not care about reproducing the idiosyncrasies and limitations of a particular bit of real-world hardware.
    * https://wiki.qemu.org/Documentation/Platforms/RISCV


## 安裝套件

## RISC-V 課程

* [Great Ideas in Computer Architecture (Machine Structures)](https://inst.eecs.berkeley.edu/~cs61c/sp19/)

## 重要文件

* [RISC-V 手册 - 一本开源指令集的指南 (PDF)](http://crva.ict.ac.cn/documents/RISC-V-Reader-Chinese-v2p1.pdf)
* [The RISC-V Instruction Set Manual Volume II: Privileged Architecture Privileged Architecture (PDF)](https://riscv.org//wp-content/uploads/2019/12/riscv-spec-20191213.pdf)
* [RISC-V Assembly Programmer's Manual](https://github.com/riscv/riscv-asm-manual/blob/master/riscv-asm.md)
* https://github.com/riscv/riscv-opcodes
    * https://github.com/riscv/riscv-opcodes/blob/master/opcodes-rv32i
* [SiFive Interrupt Cookbook (SiFive 的 RISC-V 中斷手冊)](https://gitlab.com/ccc109/sp/-/blob/master/10-riscv/mybook/riscv-interrupt/sifive-interrupt-cookbook-zh.md)
* [SiFive Interrupt Cookbook -- Version 1.0 (PDF)](https://sifive.cdn.prismic.io/sifive/0d163928-2128-42be-a75a-464df65e04e0_sifive-interrupt-cookbook.pdf)
* 進階: [proposal for a RISC-V Core-Local Interrupt Controller (CLIC)](https://github.com/riscv/riscv-fast-interrupt/blob/master/clic.adoc)

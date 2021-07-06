# qemuRiscv的用法


* 官方說明 -- https://qemu-project.gitlab.io/qemu/index.html

* https://mth.st/blog/riscv-qemu/

## 列出支援機器

```
guest@localhost:~$ qemu-system-riscv64 -machine help
Supported machines are:
none                 empty machine
sifive_e             RISC-V Board compatible with SiFive E SDK
sifive_u             RISC-V Board compatible with SiFive U SDK
spike                RISC-V Spike Board (default)
spike_v1.10          RISC-V Spike Board (Privileged ISA v1.10)
spike_v1.9.1         RISC-V Spike Board (Privileged ISA v1.9.1)
virt                 RISC-V VirtIO Board (Privileged ISA v1.10)
```

## 列出 virt 虛擬機的詳細內容

```
guest@localhost:~/test$ qemu-system-riscv64 -machine virt,dumpdtb=qemu-riscv64-virt.dtb
qemu-system-riscv64: warning: No -bios option specified. Not loading a firmware.
qemu-system-riscv64: warning: This default will change in a future QEMU release. Please use the -bios option to avoid breakages when this happens.
qemu-system-riscv64: warning: See QEMU's deprecation documentation for details.
guest@localhost:~/test$ qemu-system-riscv64 -bios none -machine virt,dumpdtb=qemu-riscv64-virt.dtb
guest@localhost:~/test$ ls
qemu-riscv64-virt.dtb
guest@localhost:~/test$ cat qemu-riscv64-virt.dtb 
&8
  �(v
riscv-virtioqemriscv-virtio,qemuchosenmuara/uart@10000000uart@10000000
�3�ive,test0000virtio_mmio@10008000
�3pvirtio,mmiovirtio_mmio@10007000
�3`virtio,mmiovirtio_mmio@10006000
�3Pvirtio,mmiovirtio_mmio@10005000
�3@virtio,mmiovirtio_mmio@10004000
�30virtio,mmiovirtio_mmio@10003000
�3 virtio,mmiovirtio_mmio@10002000
�3 virtio,mmiovirtio_mmio@10001000
orv64imafdcsucpusC���cpu-mapcluster0core0�cpu@0��7cpu3yokayafriscvaf
             friscv,sv48V;��interrupt-controller��riscv,cpu-intc���memory@800000007memory3soc
   simple-bus   ,pci@30000000N�@ !" "# !# !"8,@@@30)�7pcipci-host-ecam-generic�interrupt-controller@c000000���5�control3
                              �
                                �
riscv,clint0pat modelcompatible#size-cells#address-cellsrangesregdevice_typetimebase-frequencyclock-frequencymmu-typeriscv,isastatusphandlelinux,phandleinterrupt-controller#interrupt-cellscpuinterrupts-extendedreg-namesriscv,max-priorityriscv,ndevinterrupt-parentinterruptslinux,pci-domainbus-rangedma-coherentinterrupt-mapinterrupt-map-maskstdout-pathbootargsguest@localhost:~/test$ dtc qemu-riscv64-virt.dtb > qemu-riscv64-virt.dts
guest@localhost:~/test$ cat qemu-riscv64-virt.dts
/dts-v1/;

/ {
        #address-cells = <0x2>;
        #size-cells = <0x2>;
        compatible = "riscv-virtio";
        model = "riscv-virtio,qemu";

        chosen {
                bootargs = [00];
                stdout-path = "/uart@10000000";
        };

        uart@10000000 {
                interrupts = <0xa>;
                interrupt-parent = <0x3>;
                clock-frequency = <0x384000>;
                reg = <0x0 0x10000000 0x0 0x100>;
                compatible = "ns16550a";
        };

        test@100000 {
                reg = <0x0 0x100000 0x0 0x1000>;
                compatible = "sifive,test0";
        };

        virtio_mmio@10008000 {
                interrupts = <0x8>;
                interrupt-parent = <0x3>;
                reg = <0x0 0x10008000 0x0 0x1000>;
                compatible = "virtio,mmio";
        };

        virtio_mmio@10007000 {
                interrupts = <0x7>;
                interrupt-parent = <0x3>;
                reg = <0x0 0x10007000 0x0 0x1000>;
                compatible = "virtio,mmio";
        };

        virtio_mmio@10006000 {
                interrupts = <0x6>;
                interrupt-parent = <0x3>;
                reg = <0x0 0x10006000 0x0 0x1000>;
                compatible = "virtio,mmio";
        };

        virtio_mmio@10005000 {
                interrupts = <0x5>;
                interrupt-parent = <0x3>;
                reg = <0x0 0x10005000 0x0 0x1000>;
                compatible = "virtio,mmio";
        };

        virtio_mmio@10004000 {
                interrupts = <0x4>;
                interrupt-parent = <0x3>;
                reg = <0x0 0x10004000 0x0 0x1000>;
                compatible = "virtio,mmio";
        };

        virtio_mmio@10003000 {
                interrupts = <0x3>;
                interrupt-parent = <0x3>;
                reg = <0x0 0x10003000 0x0 0x1000>;
                compatible = "virtio,mmio";
        };

        virtio_mmio@10002000 {
                interrupts = <0x2>;
                interrupt-parent = <0x3>;
                reg = <0x0 0x10002000 0x0 0x1000>;
                compatible = "virtio,mmio";
        };

        virtio_mmio@10001000 {
                interrupts = <0x1>;
                interrupt-parent = <0x3>;
                reg = <0x0 0x10001000 0x0 0x1000>;
                compatible = "virtio,mmio";
        };

        cpus {
                #address-cells = <0x1>;
                #size-cells = <0x0>;
                timebase-frequency = <0x989680>;

                cpu-map {

                        cluster0 {

                                core0 {
                                        cpu = <0x1>;
                                };
                        };
                };

                cpu@0 {
                        linux,phandle = <0x1>;
                        phandle = <0x1>;
                        device_type = "cpu";
                        reg = <0x0>;
                        status = "okay";
                        compatible = "riscv";
                        riscv,isa = "rv64imafdcsu";
                        mmu-type = "riscv,sv48";
                        clock-frequency = <0x3b9aca00>;

                        interrupt-controller {
                                #interrupt-cells = <0x1>;
                                interrupt-controller;
                                compatible = "riscv,cpu-intc";
                                linux,phandle = <0x2>;
                                phandle = <0x2>;
                        };
                };
        };

        memory@80000000 {
                device_type = "memory";
                reg = <0x0 0x80000000 0x0 0x8000000>;
        };

        soc {
                #address-cells = <0x2>;
                #size-cells = <0x2>;
                compatible = "simple-bus";
                ranges;

                pci@30000000 {
                        interrupt-map-mask = <0x1800 0x0 0x0 0x7>;
                        interrupt-map = <0x0 0x0 0x0 0x1 0x3 0x20 0x0 0x0 0x0 0x2 0x3 0x21 0x0 0x0 0x0 0x3 0x3 0x22 0x0 0x0 0x0 0x4 0x3 0x23 0x800 0x0 0x0 0x1 0x3 0x21 0x800 0x0 0x0 0x2 0x3 0x22 0x800 0x0 0x0 0x3 0x3 0x23 0x800 0x0 0x0 0x4 0x3 0x20 0x1000 0x0 0x0 0x1 0x3 0x22 0x1000 0x0 0x0 0x2 0x3 0x23 0x1000 0x0 0x0 0x3 0x3 0x20 0x1000 0x0 0x0 0x4 0x3 0x21 0x1800 0x0 0x0 0x1 0x3 0x23 0x1800 0x0 0x0 0x2 0x3 0x20 0x1800 0x0 0x0 0x3 0x3 0x21 0x1800 0x0 0x0 0x4 0x3 0x22>;
                        ranges = <0x1000000 0x0 0x0 0x0 0x3000000 0x0 0x10000 0x2000000 0x0 0x40000000 0x0 0x40000000 0x0 0x40000000>;
                        reg = <0x0 0x30000000 0x0 0x10000000>;
                        dma-coherent;
                        bus-range = <0x0 0xff>;
                        linux,pci-domain = <0x0>;
                        device_type = "pci";
                        compatible = "pci-host-ecam-generic";
                        #size-cells = <0x2>;
                        #interrupt-cells = <0x1>;
                        #address-cells = <0x3>;
                };

                interrupt-controller@c000000 {
                        linux,phandle = <0x3>;
                        phandle = <0x3>;
                        riscv,ndev = <0x35>;
                        riscv,max-priority = <0x7>;
                        reg-names = "control";
                        reg = <0x0 0xc000000 0x0 0x4000000>;
                        interrupts-extended = <0x2 0xb 0x2 0x9>;
                        interrupt-controller;
                        compatible = "riscv,plic0";
                        #interrupt-cells = <0x1>;
                        #address-cells = <0x0>;
                };

                clint@2000000 {
                        interrupts-extended = <0x2 0x3 0x2 0x7>;
                        reg = <0x0 0x2000000 0x0 0x10000>;
                        compatible = "riscv,clint0";
                };
        };
};
```

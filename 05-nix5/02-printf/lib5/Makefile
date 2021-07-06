ifeq ($(ARCH), pc)

CC = gcc -I.
AR = ar
else 

RVCC = riscv64-unknown-elf-gcc -I. -nostdlib -fno-builtin -mcmodel=medany
ifeq ($(ARCH), riscv32)
CC = $(RVCC) -march=rv32ima -mabi=ilp32
else
CC = $(RVCC) -march=rv64gc
endif

AR = riscv64-unknown-elf-ar

endif

OUT = out/$(ARCH)
OBJS = $(OUT)/stdio.o $(OUT)/string.o
LIB = $(OUT)/lib5.a

all: $(LIB)

$(OUT)/%.o: %.c
	$(CC) -c $^ -o $@

# ar 參數參考 http://www.unixlinux.online/unixlinux/gdliunx/linuxml/201702/20299.html
$(LIB): $(OBJS)
	$(AR) -rcs $(LIB) $(OBJS)
	rm $(OBJS)

clean:
	find . -name '*.o' -delete
	find . -name '*.a' -delete
#	rm $(LIB) $(OBJS)
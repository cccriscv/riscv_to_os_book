CC := gcc -std=c99
CCC := g++ -std=c++11
LIB = cpu.cpp riscv.cpp
TARGET = asm vm dasm

all: $(TARGET)

asm: asm.cpp $(LIB)
	$(CCC) -D__ASM__ $^ -o $@

dasm: dasm.cpp $(LIB)
	$(CCC) -D__DASM__ $^ -o $@

vm: vm.cpp dasm.cpp $(LIB)
	$(CCC) -D__VM__ $^ -o $@

clean:
	rm -f $(TARGET) *.o *.exe

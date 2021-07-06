A = arch/pc
K = kernel
CC = gcc -I../lib5 -L../lib5/out/pc

all: $(A)/os

$(A)/os: $(K)/main.c
	$(CC) -l5 $^ -o $@

run: $(A)/os
	$(A)/os

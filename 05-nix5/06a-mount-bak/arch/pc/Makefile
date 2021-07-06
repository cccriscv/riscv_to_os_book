CC = gcc -I../../../lib5 -L../../../lib5/pc/
K = ../../kernel

all: main

main: $(K)/main.c
	$(CC) -l5 $^ -o $@

clean:
	rm $(TARGET) *.exe


OBJS = test
CC = gcc -I./ -I../src

all: $(OBJS)

test: ../test/test.c board.c ../src/stdio.c ../src/string.c
	$(CC) $^ -o $@

clean:
	rm *.exe $(OBJS)

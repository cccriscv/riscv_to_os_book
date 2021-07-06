OBJS = hello input printftest scanftest
all: $(OBJS)

hello: hello.c mylib.c
	gcc $^ -o $@

input: input.c mylib.c
	gcc $^ -o $@

printftest: printftest.c mylib.c
	gcc $^ -o $@

scanftest: scanftest.c mylib.c
	gcc $^ -o $@

clean:
	rm *.exe $(OBJS)

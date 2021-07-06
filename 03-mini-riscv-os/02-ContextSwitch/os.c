#include "os.h"

#define STACK_SIZE 1024
uint8_t task0_stack[STACK_SIZE];
struct context ctx_os;
struct context ctx_task;

extern void sys_switch();

void user_task0(void)
{
	lib_puts("Task0: Context Switch Success !\n");
	while (1) {} // 無窮迴圈 -- 停住！
}

int os_main(void)
{
	lib_puts("OS start\n");
	ctx_task.ra = (reg_t) user_task0;
	ctx_task.sp = (reg_t) &task0_stack[STACK_SIZE-1];
	sys_switch(&ctx_os, &ctx_task); // 行程切換 Task Switch, 從 os_main 切到 user_task0
	return 0;
}


#include "task.h"
#include "lib.h"

uint8_t task_stack[MAX_TASK][STACK_SIZE]; //  task堆疊
struct context ctx_os; // 作業系統的 context
struct context ctx_tasks[MAX_TASK]; // 各個 task 的 context
struct context *ctx_now; // 指向目前 task
int taskTop=0;  // 目前 task 數量

// create a new task // 創建新 task
int task_create(void (*task)(void))
{
	int i=taskTop++;
	ctx_tasks[i].ra = (reg_t) task;
	ctx_tasks[i].sp = (reg_t) &task_stack[i][STACK_SIZE-1];
	return i;
}

// switch to task[i]  // 切換到 task[i]
void task_go(int i) {
	ctx_now = &ctx_tasks[i];
	sys_switch(&ctx_os, &ctx_tasks[i]);
}

// switch back to os // 切換回 os
void task_os() {
	struct context *ctx = ctx_now;
	ctx_now = &ctx_os;
	sys_switch(ctx, &ctx_os);
}

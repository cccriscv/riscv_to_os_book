#include "os.h"

int os_main(void)
{
	lib_puts("OS start\n");
	timer_init(); // start timer interrupt // 啟動時間中斷

	while (1) {} // stop here ! // 無窮迴圈，停止！
	return 0;
}


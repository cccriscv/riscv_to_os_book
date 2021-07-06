#include <stdio.h>
#include "os.h"

void os_init(void) {
	vfs_init();
}

int os_start(void) {
	os_init();
	main(); // 呼叫 C 語言主程式
	while (1) {} // 讓主程式卡在這裡，用無窮迴圈停止！
	return 0;
}

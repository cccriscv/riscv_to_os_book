extern int main();

int os_main(void) {
	main(); // 呼叫 C 語言主程式
	while (1) {} // 讓主程式卡在這裡，用無窮迴圈停止！
	return 0;
}

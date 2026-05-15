#include <stdio.h>
#include "threads.h"
#include "kernel.h"
#include "syscall.h"

void start_thread(){ //初期スレッド
	create(1, 0, thread1_main);
	create(2, 1, thread2_main);
	create(3, 0, thread3_main);
	printf("start_thread : yield in\n");
	yield();
	printf("start_thread : yield out\n");
}

int main(){
	kernel_start(0, 100, start_thread); //OS動作開始(初期スレッド開始)
	return 0;
}
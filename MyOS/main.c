#include <stdio.h>
#include "threads.h"
#include "kernel.h"
#include "syscall.h"

void start_thread(){ //初期スレッド
	create(1, thread1_main);
	create(2, thread2_main);
	create(3, thread3_main);
	while(1){
		yield();
	}
}

int main(){
	kernel_start(0, start_thread); //OS動作開始(初期スレッド開始)
	return 0;
}
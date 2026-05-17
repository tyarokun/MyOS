#include <stdio.h>
#include "threads.h"
#include "kernel.h"
#include "syscall.h"

void start_thread(){ //初期スレッド
	syscall_create(1, 0, thread1_main);
	syscall_create(2, 0, thread2_main);
	syscall_create(3, 0, thread3_main);
	syscall_exit();
}

int main(){
	kernel_start(0, 100, start_thread); //OS動作開始(初期スレッド開始)
	return 0;
}
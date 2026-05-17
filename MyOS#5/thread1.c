#include <stdio.h>
#include "syscall.h"

void thread1_main(){
	int i;
	for(i = 0; i < 5; i++){
		printf("スレッド1 : for文%d周目\n", i+1);
		if((i+1) == 2){
			syscall_sleep();
		}
		syscall_yield();
	}
	syscall_exit();
}
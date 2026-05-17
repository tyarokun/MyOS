#include <stdio.h>
#include "syscall.h"

void thread2_main(){
	int i;
	for(i = 0; i < 5; i++){
		printf("スレッド2 : for文%d周目\n", i+1);
		if((i+1) == 4){
			syscall_wakeup(1);
		}
		syscall_yield();
	}
	syscall_exit();
}
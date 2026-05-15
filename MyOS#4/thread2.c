#include <stdio.h>
#include <unistd.h>
#include "syscall.h"

void thread2_main(){
	int i;
	for(i = 0; i < 5; i++){
		printf("スレッド2 : for文%d周目\n", i+1);
		sleep(1);
		yield();
	}
}
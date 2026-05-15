#include "syscall.h"
#include "kernel.h"

//スレッドから呼ばれるシステムコール
void yield(){
	syscall_param_t param;
	syscall_run(SYSCALL_TYPE_YIELD, &param);
}

void create(int id, void (*func)()){
	syscall_param_t param;
	param.un.create.id = id;
	param.un.create.func = func;
	syscall_run(SYSCALL_TYPE_CREATE, &param);
}
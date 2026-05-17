#include "syscall.h"
#include "kernel.h"

//スレッドから呼ばれるシステムコール
void syscall_exit(){
	syscall_param_t param;
	syscall_run(SYSCALL_TYPE_EXIT, &param);
}

void syscall_yield(){
	syscall_param_t param;
	syscall_run(SYSCALL_TYPE_YIELD, &param);
}

void syscall_create(int id, int priority, void (*func)()){
	syscall_param_t param;
	param.un.create.id = id;
	param.un.create.priority = priority;
	param.un.create.func = func;
	syscall_run(SYSCALL_TYPE_CREATE, &param);
}

void syscall_sleep(){
	syscall_param_t param;
	syscall_run(SYSCALL_TYPE_SLEEP, &param);
}

void syscall_wakeup(int id){
	syscall_param_t param;
	param.un.wakeup.id = id;
	syscall_run(SYSCALL_TYPE_WAKEUP, &param);
}

void syscall_chpri(int priority){
	syscall_param_t param;
	param.un.chpri.priority = priority;
	syscall_run(SYSCALL_TYPE_CHPRI, &param);
}
#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED

#include <stdint.h>
#include "syscall.h"
#define PRIORITYNUM 200	//優先度の数

typedef struct _thread_t{
  int id;
	struct _thread_t *next;
	uintptr_t *sp;
	void (*func)();
}thread_t;

struct{
	thread_t *head;
	thread_t *tail;
}ready_que;

void kernel_start(int id, void (*func)());
void kernel_schedule();
void syscall_run(syscall_type_t type, syscall_param_t *param);

#endif
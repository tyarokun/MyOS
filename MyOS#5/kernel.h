#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED

#include <stdint.h>
#include "syscall.h"
#define PRIORITYNUM 200	//優先度の数

typedef enum{
	READY = 0,
	SLEEP,
	RUNNING,
	EXIT,
}thread_state_t;

typedef struct _thread_t{
  int id;
	int priority;
	thread_state_t state;
	struct _thread_t *next;
	uintptr_t *sp;
	void (*func)();
}thread_t;

struct{
	thread_t *head;
	thread_t *tail;
}ready_que[PRIORITYNUM];

void kernel_start(int id, int priority, void (*func)());
void sysdown();
void schedule();
void syscall_run(syscall_type_t type, syscall_param_t *param);

#endif
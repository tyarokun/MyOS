#ifndef SYSCALL_H_INCLUDED
#define SYSCALL_H_INCLUDED

//システムコールのタイプ
typedef enum{
	SYSCALL_TYPE_EXIT = 0,
	SYSCALL_TYPE_YIELD,
	SYSCALL_TYPE_CREATE,
	SYSCALL_TYPE_SLEEP,
	SYSCALL_TYPE_WAKEUP,
	SYSCALL_TYPE_CHPRI,
}syscall_type_t;

//スレッドから呼ばれるシステムコールの宣言
void syscall_exit();
void syscall_yield();
void syscall_create(int id, int priprity, void(*func)());
void syscall_sleep();
void syscall_wakeup(int id);
void syscall_chpri(int priority);

//システムコールに渡すパラメータの設定
typedef struct{
	union{
		struct{
			int noret;
		}exit;
		struct{
			int ret;
		}yield;
		struct{
			int id;
			int priority;
			void (*func)();
			int ret;
		}create;
		struct{
			int ret;
		}sleep;
		struct{
			int id;
			int ret;
		}wakeup;
		struct{
			int priority;
			int ret;
		}chpri;
	}un;
}syscall_param_t;

#endif
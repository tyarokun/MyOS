#ifndef SYSCALL_H_INCLUDED
#define SYSCALL_H_INCLUDED

//システムコールのタイプ
typedef enum{
	SYSCALL_TYPE_YIELD = 0,
	SYSCALL_TYPE_CREATE,
}syscall_type_t;

//スレッドから呼ばれるシステムコールの宣言
void yield();
void create(int id, int priprity, void(*func)());

//システムコールに渡すパラメータの設定
typedef struct{
	union{
		struct{
			int ret;
		}yield;
		struct{
			int id;
			int priority;
			void (*func)();
			int ret;
		}create;
	}un;
}syscall_param_t;

#endif
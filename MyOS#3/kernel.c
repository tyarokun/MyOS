#include <stdio.h>
#include "threads.h"
#include "kernel.h"
#include "syscall.h"
#include "dispatch.h"

#define THREAD_NUM (3+1) //メインスレッドの数 + 初期スレッドの数
#define STACK_SIZE (1024 * 64)

char stack[THREAD_NUM * STACK_SIZE];

uintptr_t *kernel_sp;
thread_t *current;
thread_t *prev;
thread_t thread[THREAD_NUM];

void getcurrent(){ //実行状態にするためにready_queから外す
	ready_que.head = current->next; //レディキューのheadを次のスレッドにする
	if(ready_que.head == NULL){ //
		ready_que.tail = NULL;
	}
	current->next = NULL;
}

void putcurrent(){//実行待ち状態にする
	if(ready_que.tail == NULL){
		ready_que.head = current;
	}else{//tail == NULLのときこの操作をやる意味はないのでelseで除外する
		ready_que.tail->next = current;
	}
	ready_que.tail = current;
}

void put_thread(thread_t *thp){
	thp->next = NULL;
	if(ready_que.tail == NULL){
		ready_que.head = thp;
	}else{
		ready_que.tail->next = thp;
	}
	ready_que.tail = thp;
}
void thread_bootstrap(){//スレッドが初めて実行される時に関数を呼び出すためのもの
	current->func();
}

void kernel_create_thread(int id, void (*func)()){
	uintptr_t *sp; //ARM64のuintptr_tは64bitであり、レジスタ1つの大きさに対応している
	//スレッドの設定開始
	thread[id].id = id;
	thread[id].func = func;
	thread[id].next = NULL;
	sp = (uintptr_t *)&stack[(id + 1) * STACK_SIZE];
	sp = (uintptr_t *)((uintptr_t)sp & ~0xFUL);	//ARM64のspは64bit(16byte)境界にする必要がある→下位4ビットを0000にすることで対応
	sp -= 12; //dispatchが復元するレジスタ分の場所を先に確保している
	sp[0] = 0;	//x29
	sp[1] = (uintptr_t)thread_bootstrap; //x30
	sp[2] = 0;	//x28
	sp[3] = 0;	//x27
	sp[4] = 0;	//x26
	sp[5] = 0;	//x25
	sp[6] = 0;	//x24
	sp[7] = 0;	//x23
	sp[8] = 0;	//x22
	sp[9] = 0;	//x21
	sp[10] = 0;	//x20
	sp[11] = 0;	//x19
	thread[id].sp = sp;
	//スレッドの設定終了
	put_thread(&thread[id]);//作ったスレッドをレディキューに入れる
}

void kernel_start(int id, void (*func)()){//初期スレッド生成用
	current = NULL;
	ready_que.head = NULL;
        ready_que.tail = NULL;
        kernel_sp = NULL;
	kernel_create_thread(id, func); //初期スレッド生成
	kernel_schedule();
	dispatch(&kernel_sp, current->sp); //初期スレッドに動作を預ける
}

void kernel_schedule(){
	prev = current; //現在のスレッドをprevへ保存
	current = ready_que.head; //カレントスレッドを次のスレッドにするためにレディキューの先頭を代入
}

void syscall_run(syscall_type_t type, syscall_param_t *param){ //実際のシステムコールの処理
	switch(type){
		case SYSCALL_TYPE_YIELD:	//スレッドの切り替え処理
			getcurrent();
			putcurrent();
			kernel_schedule();
			dispatch(&prev->sp, current->sp);
			break;
		case SYSCALL_TYPE_CREATE:
			kernel_create_thread(param->un.create.id, param->un.create.func);
			break;
		default:
			break;
	}
}
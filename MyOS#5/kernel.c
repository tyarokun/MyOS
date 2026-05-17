#include <stdio.h>
#include <string.h>
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

void queue_remove_top(){ //レディ•キューの先頭のTCBを取り除く
	ready_que[current->priority].head = current->next;
	if(ready_que[current->priority].head == NULL){
		ready_que[current->priority].tail = NULL;
	}
	current->next = NULL;
}

void queue_add_entry(thread_t *thp){ //レディ•キューの末尾にTCBを接続
	thp->next = NULL;
	if(ready_que[thp->priority].tail == NULL){
		ready_que[thp->priority].head = thp;
	}else{
		ready_que[thp->priority].tail->next = thp;
	}
	ready_que[thp->priority].tail = thp;
}

void schedule(){
	int i;
	prev = current; //現在のスレッドをprevへ保存(dispatch用)
	for(i = 0; i < PRIORITYNUM; i++){//優先度が最も高い実行待機中のスレッドを検索する
		if(ready_que[i].head){
			current = ready_que[i].head; //カレントスレッドを次のスレッドにするためにレディキューの先頭を代入
			current->state = RUNNING;
			return;
		}
	}
	if(i == PRIORITYNUM){
		printf("実行できるスレッドがありません\n");
		sysdown();
	}
}

void kernel_exit(){
	printf(" スレッド%d exit\n", current->id);
	current->state = EXIT;
	queue_remove_top();
	memset(current, 0, sizeof(*current));
	schedule();
	dispatch(&prev->sp, current->sp);
}

void kernel_yield(){
		queue_remove_top();
		queue_add_entry(current);
		current->state = READY;
		printf(" スレッド%d yield\n", current->id);
		schedule();
		if(prev == current){
			/*スケジュール後スレッドが切り替わらないなら、ディスパッチしない。
			レジスタ退避はCPUが実行中のスレッドに対して行うので
			実行中スレッドのspのアドレスしか第一引数に渡せない。
			また、レジスタ復旧は過去にディスパッチされたスレッドに対して行う前提だが、
			current->spを渡すとまだ退避されていないスレッドのspを渡したことになってしまう。
			*/
			current->state = RUNNING;
			return;
		}else{
			dispatch(&prev->sp, current->sp);
		}
}

void kernel_sleep(){
	current->state = SLEEP;
	queue_remove_top();
	printf(" スレッド%d sleep\n", current->id);
	schedule();
	dispatch(&prev->sp, current->sp);
}

void kernel_wakeup(int id){
	thread_t *thp;
	thp = &thread[id];
	if(thp->state == SLEEP){ //SLEEP以外のときにwakeupしてしまうと同じスレッドが2つ以上レディ•キューに入ってしまう可能性がある
		printf(" スレッド%d wakeup\n", thp->id);
		thp->state = READY;
		queue_add_entry(thp);
	}
	else{
		return;
	}
}

void kernel_chpri(int priority){
	queue_remove_top();
	if(current->priority != priority){
		printf(" スレッド%d chpri %d → %d\n", current->id, current->priority, priority);
		current->priority = priority;
	}
	queue_add_entry(current);
	schedule();
	dispatch(&prev->sp, current->sp);
}

void thread_bootstrap(){//スレッドが初めて実行される時に関数を呼び出すためのもの
	current->func();
	kernel_exit();
}

void kernel_create(int id, int priority, void (*func)()){
	uintptr_t *sp; //ARM64のuintptr_tは64bitであり、レジスタ1つの大きさに対応している
	//スレッドの設定開始
	thread[id].id = id;
	thread[id].priority = priority;
	thread[id].state = READY;
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
	queue_add_entry(&thread[id]);//作ったスレッドをレディキューに入れる
}

void kernel_start(int id, int priority, void (*func)()){//初期スレッド生成 (開始地点なのでここだけ「スレッド→システムコール→カーネル」のポリシー無視)
	current = NULL;
	kernel_sp = NULL;
	for(int i = 0; i < PRIORITYNUM; i++){
		ready_que[i].head = NULL;
		ready_que[i].tail = NULL;
	}
	kernel_create(id, priority, func); //初期スレッド生成
	schedule();
	dispatch(&kernel_sp, current->sp); //初期スレッドに動作を預ける
}

void sysdown(){
	printf("system error\n");
	while(1);
}

void syscall_run(syscall_type_t type, syscall_param_t *param){ //実際のシステムコールの処理
	switch(type){
		case SYSCALL_TYPE_EXIT:
			kernel_exit();
			break;
		case SYSCALL_TYPE_YIELD:	//スレッドの切り替え処理
			kernel_yield();
			break;
		case SYSCALL_TYPE_CREATE:
			kernel_create(param->un.create.id, param->un.create.priority, param->un.create.func);
			break;
		case SYSCALL_TYPE_SLEEP:
			kernel_sleep();
			break;
		case SYSCALL_TYPE_WAKEUP:
			kernel_wakeup(param->un.wakeup.id);
			break;
		case SYSCALL_TYPE_CHPRI:
			kernel_chpri(param->un.chpri.priority);
			break;
		default:
			break;
	}
}
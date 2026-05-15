.text
.global _dispatch
.align 2

_dispatch:
  //x0 : 第1引数(*current->sp)
  //x1 : 第2引数(*next->sp)

  //現在のスレッドのレジスタを退避
  stp x19, x20, [sp, #-16]!
  stp x21, x22, [sp, #-16]!
  stp x23, x24, [sp, #-16]!
  stp x25, x26, [sp, #-16]!
  stp x27, x28, [sp, #-16]!
  stp x29, x30, [sp, #-16]!

  /*
  stp x19, x20, [sp, #-16]! の意味
  sp = sp - 16;
  メモリ[sp]     = x19;
  メモリ[sp + 8] = x20;
  */

  //currentのspへ現在のspを保存 (current->sp = CPUのsp)
  mov x9, sp    //str命令はspを直接扱えないので1度x9に代入して使用する
  str x9, [x0]

  //nextのspへ切り替える (CPUのsp = next->sp)
  mov sp, x1

  //nextのコンテキストを復元
  ldp x29, x30, [sp], #16
  ldp x27, x28, [sp], #16
  ldp x25, x26, [sp], #16
  ldp x23, x24, [sp], #16
  ldp x21, x22, [sp], #16
  ldp x19, x20, [sp], #16

  ret //x30に入っているアドレスへジャンプ
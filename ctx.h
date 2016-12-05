/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:ctx.h
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-02
*   Describe:
*
********************************************************/
#ifndef _CTX_H
#define _CTX_H


/*  栈空间地址是由高向低变化
 *   High     // ebp > esp  
 *   | | 
 *   | |  
 *   | |  funcA call funcB
 *   | |_________
 *   | |  栈底
 *   | | 
 *   | |  funcB
 *   | |
 *   | |  栈顶
 *   Low 
 */

#include<setjmp.h>
#include<stdint.h>
#include<stdlib.h>


typedef struct  context
{
    jmp_buf        _ctx;
    char     *     _stack_addr;      //协程的栈空间, 栈顶
    uint32_t       _stack_size;       //协程的栈大小
}context;

extern "C"  int save_context(jmp_buf jbf);
extern "C"  void restore_context(jmp_buf jbf, int ret);
extern "C"  void replace_esp(jmp_buf jbf, void* esp);



int   init_context(context *p , char * stack_addr,  uint32_t stack_size);

int   save_context(context  *p);

void  resume_context(context *p);

void  swap_context(context *ctx, context *new_ctx );


 
#endif //CTX_H

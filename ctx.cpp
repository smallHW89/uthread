/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:ctx.cpp
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-02
*   Describe:
*
********************************************************/
#include "ctx.h"

extern "C"  int save_context(jmp_buf jbf);
extern "C"  void restore_context(jmp_buf jbf, int ret);
extern "C"  void replace_esp(jmp_buf jbf, void* esp);




int save_context( context *p )
{
    return save_context(p->_ctx);
}

void  resume_context( context *p )
{
    return restore_context(p->_ctx, 1 ) ;
}


int init_context(context *p ,  char * stack_addr , uint32_t stack_size)
{
    p->_stack_addr = stack_addr;
    p->_stack_size = stack_size;
    char * stack_base = stack_addr + stack_size; 
    
    int ret = save_context( p ); //保存寄存器上下文
    if( 0 == ret )
    {
        replace_esp(p->_ctx, (void*)(stack_base) );//用自己的栈空间 
        return 0;
    }
    else
    {
        return 1;
    }
}

void  swap_context(context *ctx, context *new_ctx )
{
    int ret = save_context(ctx);
    if( 0 == ret )
    {
        resume_context( new_ctx );
    }
}

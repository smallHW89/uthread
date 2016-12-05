/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:uthread.h
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-02
*   Describe:
*
********************************************************/
#ifndef _UTHREAD_H
#define _UTHREAD_H

#include <stdint.h>
#include<stdlib.h>
#include <map>
#include <set>
#include <list>

#include "ctx.h"

//协程入口函数
typedef  void (*UthreadEntry) (void *);

typedef  enum{
    UTHREAD_STATE_UNINIT   = -1,
    UTHREAD_STATE_INIT     = 0,
    UTHREAD_STATE_RUNABLE  = 1,
    UTHREAD_STATE_RUNNING  = 2, 
    UTHREAD_STATE_SLEEPING = 3,
    UTHREAD_STATE_PENDING ,
    UTHREAD_STATE_DIE      
}e_uthread_enum;

class UnCopyAble
{
public:
    UnCopyAble(){};
    ~UnCopyAble(){};
private:
    UnCopyAble(const UnCopyAble &);
    UnCopyAble & operator= (const UnCopyAble &);
};

class scheduler;

class uthread : public UnCopyAble{
public:
    uthread (scheduler *sched, uint32_t id, UthreadEntry func, void *arg  ):
        _pid(id), _thread_func(func), _arg(arg),
        _stack_addr(NULL), _stack_size(0),_state(UTHREAD_STATE_UNINIT), _sched(sched)
    {
        _stack_size =  _default_stack_size; 
        _stack_addr = new char[_stack_size];
    }

    uthread (scheduler *sched,uint32_t id, UthreadEntry func, void *arg, uint32_t stack_size  ):
        _pid(id), _thread_func(func), _arg(arg), 
        _stack_addr(NULL), _stack_size(stack_size),_state(UTHREAD_STATE_UNINIT), _sched(sched)
    {
        _stack_size =  _default_stack_size; 
        _stack_addr = new char[_stack_size];
    }

    ~uthread()
    {
        if(NULL != _stack_addr )       
        {
            delete []_stack_addr;
            _stack_addr = NULL;
        }
    }
 
    void run();

    uint32_t get_pid(){ return _pid; }
    uint32_t get_state() {return _state ; }
    void     set_state(e_uthread_enum e) { _state = e ;}

    static const uint32_t _default_stack_size = 128*1024;
    friend class scheduler;
private:
    UthreadEntry    _thread_func;
    void        *   _arg;
    e_uthread_enum  _state;
    uint32_t        _pid;
    context         _ctx;
    char        *   _stack_addr;
    uint32_t        _stack_size;
    scheduler   *   _sched;

};



#endif //UTHREAD_H

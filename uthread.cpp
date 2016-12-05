/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:uthread.cpp
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-02
*   Describe:
*
********************************************************/
#include "uthread.h"
#include <stdio.h>

void uthread::run()
{
    if( NULL != _thread_func  )
    {
        printf("thread id:%u begin\n", _pid); 
        _state = UTHREAD_STATE_RUNNING;
        _thread_func(_arg);
    }
    _state = UTHREAD_STATE_DIE;
}



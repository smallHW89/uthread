/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:uthread_main.cpp
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-04
*   Describe:
*
********************************************************/
#include<stdio.h>
#include<stdlib.h>
#include "uthread_main.h"
#include "uthread.h" 
#include "sched.h"


/*
 * 开发者实现
 */
extern void  uthread_main( void * );

scheduler     g_sched;

void go_sleep(uint32_t ms)
{
    g_sched.sleep(ms);
}

void go_yield()
{
    g_sched.yield();
}

void go_thread(UthreadEntry func, void *arg, uint32_t size)
{
    g_sched.create_thread(func, arg, size);
}

ssize_t go_udpsend(int fd, char *buf, size_t len , uint32_t ot)
{
    return g_sched.udp_send(fd, buf, len, ot);
}

ssize_t go_udprecv(int fd, char *buf, size_t len , uint32_t ot)
{
    return g_sched.udp_recv(fd, buf, len , ot);
}


int main()
{
    g_sched.scheduler_init();
    int pid = g_sched.create_thread(uthread_main, NULL );
    printf("start main:%u\n", pid);
    //pid  = g_sched.create_thread(uthread_mainB , NULL);
    //printf("start main:%u\n", pid);
    g_sched.loop(); 
}

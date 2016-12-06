/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:uthread_main.h
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-04
*   Describe:
*
********************************************************/
#ifndef _UTHREAD_MAIN_H
#define _UTHREAD_MAIN_H
#include "uthread.h" 
#include "sched.h"


void     go_sleep(uint32_t ms);
void     go_yield();
void     go_thread(UthreadEntry func, void *arg, uint32_t size = 0);
ssize_t  go_udpsend(int fd, char *buf, size_t len, uint32_t ot, const struct sockaddr * dest_addr,  socklen_t addrlen);
ssize_t  go_udprecv(int fd, char *buf, size_t len, uint32_t ot, struct sockaddr * src_addr, socklen_t  * addrlen );


#endif //UTHREAD_MAIN_H

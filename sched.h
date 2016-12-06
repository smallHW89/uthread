/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:sched.h
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-02
*   Describe:
*
********************************************************/
#ifndef _SCHED_H
#define _SCHED_H

#include<sys/types.h>
#include<sys/socket.h>

#include "uthread.h"

#include "ctimer.h"

#include "poll_mgr.h"

class scheduler : public UnCopyAble {

public:
    scheduler():_nextpid(0)
    {
    }
    ~scheduler()
    {
    }

    int scheduler_init();

    uint32_t  create_thread(UthreadEntry func, void *arg, uint32_t size= 0  );
    void  sleep(uint32_t ms);
    void  yield(); //当前协程让出
    void  loop(); //主协程，循环，执行调度算法
    void  add_runnable_list(uthread * u) { _runnable_list.push_back(u);  }
    
    /*
     * 网络接口，该阻塞时，切换协程
     * tcp: connect, accept, read, send 
     * udp: read, send
     */

    ssize_t udp_send(int fd, char *buf, size_t len, uint32_t  otms) ;
    ssize_t udp_send(int fd, char *buf, size_t len, uint32_t  otms, const struct  sockaddr * dest_addr , socklen_t addrlen);
    ssize_t udp_recv(int fd, char *buf, size_t len, uint32_t  otms, struct sockaddr * src_addr, socklen_t * addrlen);
    
private:
    uint32_t                        _nextpid; 
    std::map<uint32_t , uthread * > _thread_map;
    context                         _primo_ctx;
    static  uthread *               _cur_thread;
    std::list<uthread*>             _runnable_list;
    std::list<uthread*>             _dead_list;

    CTimerMng<uthread >            _sleep_mng;
    PollMgr                        _io_mgr;
    CTimerMng<uthread >            _timer_mgr;

};

 
#endif //SCHED_H

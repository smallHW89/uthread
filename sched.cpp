/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:sched.cpp
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-02
*   Describe:
*
********************************************************/
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<stdio.h>

#include "sched.h"
#include "util.h"
#include "ctimer.h"
#include "ctx.h"

uthread * scheduler::_cur_thread(NULL) ;

int scheduler :: scheduler_init()
{
    _io_mgr.InitPoll(10000);
    return 0;
}

/*
 * return pid
 */
uint32_t  scheduler :: create_thread(UthreadEntry func, void *arg, uint32_t size )
{
    uthread * p = NULL;
    uint32_t pid = 0;
    if( _nextpid == 0 )
        _nextpid ++;
    pid = _nextpid;
    _nextpid++;
    if( 0 == size )
        p = new uthread(this, pid, func, arg);
    else
        p = new uthread(this, pid, func, arg, size);
    _thread_map[pid] = p;

    p->_ctx._stack_addr = p->_stack_addr;
    p->_ctx._stack_size = p->_stack_size;
    char * stack_base = p->_stack_addr + p->_stack_size; 
    
    add_runnable_list(p);
    int ret = save_context( p->_ctx._ctx ); //保存寄存器上下文
    if( 0 == ret )
    {
        printf("create a new thread:%u\n", pid);
        replace_esp(p->_ctx._ctx, (void*)(stack_base) );//用自己的栈空间 
        return pid;
    }
    else
    {
        printf("pid %u begin\n", _cur_thread->get_pid());
        if(NULL != _cur_thread)
            _cur_thread->run();
        //回收线程
        printf("pid %u end\n", _cur_thread->get_pid());
        _cur_thread->_state = UTHREAD_STATE_DIE;
        _cur_thread->_sched->_dead_list.push_back(_cur_thread); 
        _cur_thread->_sched->yield();
        return 0;
    }
}

/*
 * 当前线程让出CPU, 切到primo线程
 */
void scheduler :: yield()
{
    uthread * pu = _cur_thread;

    int ret = save_context( pu->_ctx._ctx);
    if( 0 == ret )
    {
         restore_context( _primo_ctx._ctx, 1);
    }
}

/*
 * 
 */
void scheduler :: sleep(uint32_t ms)
{
    uint64_t  timestamp = get_current_ms() ;    
    uint64_t  ot =  timestamp += ms;
    uthread * pu = _cur_thread;
    //printf("id:%u scheule sleep :%llu, %llu, %u\n",pu->get_pid(), timestamp,ot,ms); 


    CTimerObj<uthread>   * obj =new CTimerObj<uthread> (pu, ot);

    //printf("sleep_mgr count:%u  \n", _sleep_mng.GetObjCount()  );  
    _sleep_mng.AddTimerObj( obj );
    //printf("sleep_mgr count:%u  \n", _sleep_mng.GetObjCount()  );  

    _cur_thread->_state =  UTHREAD_STATE_SLEEPING;
    yield();
    timestamp = get_current_ms() ;    
    //printf("id:%u scheule wake :%llu\n",_cur_thread->get_pid(), timestamp); 
    //printf("sleep_mgr count:%u  \n", _sleep_mng.GetObjCount()  );  
    _sleep_mng.DelTimeObj( obj );
    //printf("sleep_mgr count:%u  \n", _sleep_mng.GetObjCount()  );  
    _cur_thread->_state =  UTHREAD_STATE_RUNNING;
}



void scheduler :: loop()
{
    while( _thread_map.size()> 0 )  
    {
        //可运行线程，调度去运行
        while( _runnable_list.size()> 0  )
        {
            uthread * pu = _runnable_list.front();
            _runnable_list.pop_front();
            _cur_thread =  pu;  
            //printf("save _primo_ctx\n");
            int ret = save_context(  _primo_ctx._ctx );
            if( 0 == ret)
            {
                //printf("resume pid:%u ctx\n", _cur_thread->get_pid());
                restore_context(_cur_thread->_ctx._ctx, 1);     
            }
        }
        
        //sleep线程，如果达到sleep时间， 加入runnable_list
        uint64_t cur_t = get_current_ms();   
        std::vector< CTimerObj<uthread>* >  obj_list;
        int ii = _sleep_mng.GetTimeOutObj( obj_list , cur_t );
        for(int i=0; i< obj_list.size(); i ++ )
        {
            uthread *pu =  obj_list[i]->GetObj(); 
            add_runnable_list(pu);
        }
        obj_list.clear();

        //处理网络fd, fifo事件
        uint64_t min_ot1 = _sleep_mng.GetMinTimeOut();
        uint64_t min_ot2 = _timer_mgr.GetMinTimeOut();
        uint64_t min_ot =0 ;
        if(min_ot1 == 0 ) min_ot = min_ot2;
        else  if(min_ot2 == 0) min_ot = min_ot1;
        else  min_ot = (min_ot1 < min_ot2)? min_ot1:min_ot2;

        uint64_t tt = 0;
        if( min_ot == 0 )         tt = 10; 
        else if( min_ot >= cur_t) tt = min_ot - cur_t;
        std::vector<uthread* > t_list;
        _io_mgr.WaitEvent(tt, t_list  );
        for(int i=0; i< t_list.size(); i++ )
        {
            uthread *pu = t_list[i];
            add_runnable_list(pu);
        }

        //判断定时器，对于超时的线程,唤醒
        {
            uint64_t cur_t = get_current_ms();   
            std::vector< CTimerObj<uthread>* >  obj_list;
            _timer_mgr.GetTimeOutObj( obj_list , cur_t);
            for(int i=0; i< obj_list.size(); i ++ )
            {
                uthread *pu =  obj_list[i]->GetObj(); 
                add_runnable_list(pu);
            }
        }

        //对die协程，回收
        while(_dead_list.size() > 0 )
        {
            uthread * pu = _dead_list.front();
            _dead_list.pop_front();
            _thread_map.erase(pu->get_pid() );
            printf("pid %u delete\n", pu->get_pid());
            delete pu;
        }
    }
}

ssize_t scheduler :: udp_send(int fd, char  *buf, size_t len, uint32_t otms, const struct sockaddr * dest_addr, socklen_t addrlen)
{
    uthread *pu = _cur_thread;
    int events =  EPOLLOUT;
    _io_mgr.PollCtlAdd(fd, events, pu );
    uint64_t  timestamp = get_current_ms() ; 
    timestamp += otms;
    CTimerObj<uthread>   obj(pu, timestamp);
    if(otms)
    {
        _timer_mgr.AddTimerObj( &obj );
    }
    pu->_state = UTHREAD_STATE_PENDING ;
    yield(); //切走，fd可读写、或者超时后唤醒
    if(otms)
    {
        _timer_mgr.DelTimeObj( &obj );
    }
    pu->_state = UTHREAD_STATE_RUNNING;

    //判断fd 的事件
    int revents = _io_mgr.GetFdEvents(fd);
    _io_mgr.SetFdEvents(fd, revents&~EPOLLOUT);
    _io_mgr.PollCtlDel(fd, EPOLLOUT, pu);

    if ( otms &&  (revents & EPOLLOUT == 0) )
    {
        errno = ETIME;
        return -1;
    }

    ssize_t rn = ::sendto(fd, buf, len, 0 , dest_addr, addrlen); 
    return  rn;
}

ssize_t scheduler :: udp_send(int fd, char  *buf, size_t len, uint32_t otms)
{
    uthread *pu = _cur_thread;
    int events =  EPOLLOUT;
    _io_mgr.PollCtlAdd(fd, events, pu );
    uint64_t  timestamp = get_current_ms() ; 
    timestamp += otms;
    CTimerObj<uthread>   obj(pu, timestamp);
    if(otms)
    {
        _timer_mgr.AddTimerObj( &obj );
    }
    pu->_state = UTHREAD_STATE_PENDING ;
    yield(); //切走，fd可读写、或者超时后唤醒
    if(otms)
    {
        _timer_mgr.DelTimeObj( &obj );
    }
    pu->_state = UTHREAD_STATE_RUNNING;

    //判断fd 的事件
    int revents = _io_mgr.GetFdEvents(fd);
    _io_mgr.SetFdEvents(fd, revents&~EPOLLOUT);
    _io_mgr.PollCtlDel(fd, EPOLLOUT, pu);

    if ( otms &&  (revents & EPOLLOUT == 0) )
    {
        errno = ETIME;
        return -1;
    }

    ssize_t rn = ::send(fd, buf, len, 0 ); 
    return  rn;
}

ssize_t scheduler::udp_recv(int fd, char *buf, size_t len, uint32_t otms, struct sockaddr * src_addr, socklen_t * addrlen)
{
    uthread *pu = _cur_thread;
    int events =  EPOLLIN;
    _io_mgr.PollCtlAdd(fd, events, pu );
    uint64_t  timestamp = get_current_ms() ; 
    timestamp += otms;
    CTimerObj<uthread>   obj(pu, timestamp);
    if(otms)
    {
        _timer_mgr.AddTimerObj( &obj );
    }
    pu->_state = UTHREAD_STATE_PENDING ;
    yield(); //切走，fd可读、或者超时后唤醒
    if(otms)
    {
        _timer_mgr.DelTimeObj( &obj );
    }
    pu->_state = UTHREAD_STATE_RUNNING;

    //判断fd 的事件
    int revents = _io_mgr.GetFdEvents(fd);
    _io_mgr.SetFdEvents(fd, revents&~EPOLLIN);
    _io_mgr.PollCtlDel(fd, EPOLLIN, pu);

    if ( otms &&  (revents & EPOLLIN == 0) )
    {
        errno = ETIME;
        return -1;
    }

    ssize_t rn = ::recvfrom(fd, buf, len, 0 , src_addr, addrlen); 
    return  rn;
}





/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:poll_mgr.h
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-03
*   Describe:
*
********************************************************/
#ifndef _POLL_MGR_H
#define _POLL_MGR_H
 
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#include <list>
#include <vector>

#include "uthread.h"

typedef struct  PollObj
{
    PollObj(int fd, int events):_fd(fd), _events(events), _revents(0), _r_cnt(0), _w_cnt(0)
    {}
    int _fd;
    int _events;
    int _revents;
    int _r_cnt;
    int _w_cnt;
    std::list<uthread*>  _r_list;
    std::list<uthread*>  _w_list;
}FdObj;


class PollMgr
{
public:
    PollMgr()   ;
    ~PollMgr()  ;

    bool InitPoll(int max_fd_num);
    bool PollCtlAdd(int fd, int events, uthread * t);
    bool PollCtlDel(int fd, int events, uthread * t);
    bool PollDel(int fd, uthread *t );
    void WaitEvent(int ms, std::vector<uthread* > & t_list );
    int  GetFdEvents(int fd );
    int  SetFdEvents(int fd , int event);
    
    static const int         MAX_FD_NUM = 100000; 
private:
    int                       _ep_fd;
    int                       _max_fd;
    std::map<int , PollObj* > _poll_map;
    struct epoll_event      * _ev_list;  

};
 






#endif //POLL_MGR_H

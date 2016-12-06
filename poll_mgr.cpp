/********************************************************
 *   Copyright (C) 2016 All rights reserved.
 *   
 *   Filename:poll_mgr.cpp
 *   Author  :huangwei  497225735@qq.com
 *   Date    :2016-12-03
 *   Describe:
 *
 ********************************************************/
#include "poll_mgr.h"

PollMgr:: PollMgr():_ep_fd(-1), _max_fd(MAX_FD_NUM), _ev_list(NULL)
{
}

PollMgr:: ~PollMgr()
{
    if( _ev_list != NULL )
        delete []_ev_list;
}

bool PollMgr :: InitPoll(int  max_fd_num)
{
    _max_fd = max_fd_num;
    if(max_fd_num <=0  ||  max_fd_num > MAX_FD_NUM )
        _max_fd = MAX_FD_NUM;

    _ep_fd = epoll_create(_max_fd);
    if( _ep_fd < 0 )
        return false;

    _ev_list = new  struct epoll_event[_max_fd] ;
    return true;
}

int PollMgr::GetFdEvents(int fd)
{
    std::map<int , PollObj *>::iterator iter;
    iter = _poll_map.find(fd);
    if( iter  == _poll_map.end() ) 
        return 0;
    return iter->second->_revents;
}

int PollMgr::SetFdEvents(int fd, int event)
{
    std::map<int , PollObj *>::iterator iter;
    iter = _poll_map.find(fd);
    if( iter  == _poll_map.end() ) 
        return 0;
    iter->second->_revents = event; 
    return 0;
}



bool PollMgr :: PollCtlAdd(int fd, int events, uthread * t)
{
    FdObj * po = NULL;
    std::map<int , PollObj *>::iterator iter;
    int  old_events = 0 ;
    int  new_events = 0 ;
    iter = _poll_map.find(fd);
    if( iter  == _poll_map.end() ) 
    {
        po = new  PollObj(fd, events);
        _poll_map[fd] = po;
        old_events  =  0;
    }
    else 
    {
        po = iter->second;
        old_events  = po->_events; 
    }

    std::list<uthread *>::iterator   it ;
    if (events & EPOLLIN )  
    {
        bool b = false;
        for( it = po->_r_list.begin(); it != po->_r_list.end(); it++ )
        {
            if( (*it)->get_pid() == t->get_pid() )  b = true;
        }
        if( false == b )
        {
            po->_r_list.push_back(t);
            po->_r_cnt++;
        }
    }
    if (events & EPOLLOUT )  
    {
        bool b = false;
        for( it = po->_w_list.begin(); it != po->_w_list.end(); it++ )
        {
            if( (*it)->get_pid() == t->get_pid() )  b = true;
        }
        if( false == b )
        {
            po->_w_list.push_back(t);
            po->_w_cnt ++;
        }
    }

    new_events = old_events;
    if( po->_r_cnt )
        new_events = new_events | EPOLLIN;
    if( po->_w_cnt )
        new_events = new_events | EPOLLOUT;

    po->_events = new_events;

    if( old_events == new_events )
    {
        return true;
    }
    int op = old_events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    struct epoll_event  ev;
    ev.events = new_events;
    ev.data.fd = fd;
    if (epoll_ctl(_ep_fd, op, fd, &ev) < 0)  
    {
        return false;
    }
    return true;
}

bool PollMgr :: PollCtlDel(int fd, int events , uthread *t)
{
    FdObj * po = NULL;
    std::map<int , PollObj *>::iterator iter;
    int  old_events = 0 ;
    int  new_events = 0 ;
    iter = _poll_map.find(fd);
    if( iter  == _poll_map.end() ) 
    {
        return false;
    }
    else 
    {
        po = iter->second;
        old_events  = po->_events; 
    }

    std::list<uthread *>::iterator   it ;
    if (events & EPOLLIN )  
    {
        bool b = false;
        for( it = po->_r_list.begin(); it != po->_r_list.end(); it++ )
        {
            if( (*it)->get_pid() == t->get_pid() ) { b = true; break; }
        }
        if (true == b )
        {
            po->_r_list.erase(it);
            po->_r_cnt--;
        }
    }
    if (events & EPOLLOUT )  
    {
        bool b = false;
        for( it = po->_w_list.begin(); it != po->_w_list.end(); it++ )
        {
            if( (*it)->get_pid() == t->get_pid() )  {b = true; break; }
        }
        if( true == b )
        {
            po->_w_list.erase(it);
            po->_w_cnt--;
        }
    }

    new_events = old_events;
    if( po->_r_cnt == 0 )
        new_events  = new_events & (~ EPOLLIN);
    if( po->_w_cnt == 0 )
        new_events = new_events & ( ~EPOLLOUT);

    po->_events = new_events;

    if( old_events == new_events )
    {
        return true;
    }
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    struct epoll_event  ev;
    ev.events = new_events;
    ev.data.fd = fd;
    if ((epoll_ctl(_ep_fd, op, fd, &ev) < 0) )
    {
        return false;
    }
    
    if( po->_r_cnt + po->_w_cnt == 0 )
    {
        _poll_map.erase(fd);
        delete po;
    }
    return true;
}

bool PollMgr :: PollDel(int fd, uthread * t)
{
    FdObj * po = NULL;
    std::map<int , PollObj *>::iterator iter;
    int  old_events = 0 ;
    int  new_events = 0 ;
    iter = _poll_map.find(fd);
    if( iter  == _poll_map.end() ) 
    {
        return true;
    }
    else 
    {
        po = iter->second;
        old_events  = po->_events; 
    }
    
    std::list<uthread *>::iterator   it ;
    {
        bool b = false;
        for( it = po->_r_list.begin(); it != po->_r_list.end(); it++ )
        {
            if( (*it)->get_pid() == t->get_pid() ) { b = true; break; }
        }
        if (true == b )
        {
            po->_r_list.erase(it);
            po->_r_cnt--;
        }
    }
    {
        bool b = false;
        for( it = po->_w_list.begin(); it != po->_w_list.end(); it++ )
        {
            if( (*it)->get_pid() == t->get_pid() )  {b = true; break; }
        }
        if( true == b )
        {
            po->_w_list.erase(it);
            po->_w_cnt--;
        }
    }

    new_events = old_events;
    if( po->_r_cnt == 0 )
        new_events  = new_events & (~ EPOLLIN);
    if( po->_w_cnt == 0 )
        new_events = new_events & ( ~EPOLLOUT);

    po->_events = new_events;

    if( old_events == new_events )
    {
        return true;
    }
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    struct epoll_event  ev;
    ev.events = new_events;
    ev.data.fd = fd;
    if ((epoll_ctl(_ep_fd, op, fd, &ev) < 0) )
    {
        return false;
    }
    
    if( po->_r_cnt + po->_w_cnt == 0 )
    {
        _poll_map.erase(fd);
        delete po;
    }
    return true;
}

void PollMgr:: WaitEvent(int ms , std::vector<uthread* > &t_list )
{
    int nfd = epoll_wait(_ep_fd, _ev_list, _max_fd, ms);
    if( nfd <= 0 )
        return ;

    std::set<int>   pid_set; 
    for(int i=0; i< nfd; i ++ )
    {
        int revents =  _ev_list[i].events;  
        int fd      =  _ev_list[i].data.fd;

        FdObj * po = NULL;
        std::map<int , PollObj *>::iterator iter;
        iter = _poll_map.find(fd);
        if( iter  == _poll_map.end() ) 
        {
            continue;
        }
        po = iter->second;
        po->_revents = revents;

        if( revents & EPOLLIN  && po->_r_cnt > 0 )
        {
            uthread * pu = po->_r_list.front();
            if( pid_set.find(pu->get_pid())  == pid_set.end()  )
                t_list.push_back(pu);
            pid_set.insert(pu->get_pid() );
        }
        if( revents & EPOLLOUT && po->_w_cnt > 0 )
        {
            uthread * pu = po->_w_list.front();
            if( pid_set.find(pu->get_pid())  == pid_set.end()  )
                t_list.push_back(pu);
            pid_set.insert(pu->get_pid() );
        }
    }
}

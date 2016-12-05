/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:ctimer.h
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-11-06
*   Describe: timer , 定时器，用multimap实现
*
********************************************************/
#ifndef _CTIMER_H
#define _CTIMER_H

#include<stdint.h>
#include<stdlib.h>
#include<map>
#include<vector>

typedef int ( * TimeOutFunc ) (void *);

template <typename Obj> class CTimerObj;
template <typename Obj> class CTimerMng;



template <typename Obj>
class  CTimerObj
{
public:
    CTimerObj(Obj * p  ): _pobj(p) ,         _expire_timestamp(0) {}
    CTimerObj(Obj * p , uint64_t t):_pobj(p), _expire_timestamp(t) {}
    void  SetExpireTimestamp(uint64_t t)
    {
        _expire_timestamp = t;
    }
    uint64_t GetExpireTimestamp()
    {
        return _expire_timestamp;
    }

    virtual int  OnTimeOut()/*子类重写该接口*/
    {
        return 0;
    }

    Obj * GetObj()
    {
        return _pobj;
    }

    friend  class CTimerMng<Obj>;

protected:

private:
    uint64_t    _expire_timestamp; //
    Obj       * _pobj;
    typename std::multimap<uint64_t, CTimerObj<Obj> * >::iterator _iterator; 
    //TimerIter _iterator;
};

template <typename Obj>
class CTimerMng
{
    public:
        typename std::multimap<uint64_t, CTimerObj<Obj> *     >  ::iterator AddTimerObj(CTimerObj<Obj> * obj)
        {
            typename std::multimap<uint64_t, CTimerObj<Obj> *>::iterator    iter = _timer_list.insert(std::make_pair(obj->_expire_timestamp, obj));   
            obj->_iterator = iter;
            return obj->_iterator;
        }

        void      DelTimeObj( CTimerObj<Obj> *obj)
        {
            _timer_list.erase(obj->_iterator); 
        }

        int       GetTimeOutObj(std::vector<CTimerObj<Obj> *  >  & objList, uint64_t timeLine)
        {
            int i=0;
            typename std::multimap<uint64_t, CTimerObj<Obj> *>::iterator  iter = _timer_list.begin();
            for( ; iter != _timer_list.end(); iter++ )
            {
                if(iter->first <= timeLine ) 
                {
                    objList.push_back(iter->second);
                    i++;
                }
                else
                {
                    break;
                }
            }
            return i;

        }

        uint64_t  GetMinTimeOut()
        {
            if( _timer_list.size() > 0 )
            {
                return _timer_list.begin()->first;
            }
            else
                return 0;
        }

        uint64_t GetObjCount()
        {
            return _timer_list.size();
        }

    private:
        std::multimap<uint64_t, CTimerObj<Obj> *>  _timer_list;

};

#endif //CTIMER_H

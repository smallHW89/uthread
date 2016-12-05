/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:util.cpp
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-03
*   Describe:
*
********************************************************/
#include<sys/time.h>

#include "util.h"

uint64_t  get_current_ms()
{
    struct timeval  tv;
    gettimeofday(&tv , NULL);

    uint64_t ms = tv.tv_sec * 1000 ;
    ms = ms + (tv.tv_usec/1000);
    return ms ;
}

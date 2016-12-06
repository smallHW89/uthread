/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:test_app.cpp
*   Author  :huangwei  497225735@qq.com
*   Date    :2016-12-04
*   Describe:
*
********************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "uthread_api.h"


void test_thread(void *)
{
    printf("test thread begin \n");

    for(int i=0; i<10; i++ )
    {
        go_sleep(5000);
        printf("test func %u \n", i); 
    }

}

void uthread_mainB(void *)
{
    printf("uthread main begin %llu\n",  get_current_ms());

    //go_thread(test_thread,NULL);

    for(int i=0; i< 10; i++)
    {
        uint64_t curt = get_current_ms();
        go_sleep(2000); 
        printf("mainB sleep  wake i %u, %llu\n", i, get_current_ms());
    }

    return ;
}


/*
 * 开发者实现这个
 */
void uthread_main(void *)
{
    printf("uthread main begin %llu\n",  get_current_ms());

    go_thread(test_thread,NULL);

    for(int i=0; i< 10; i++)
    {
        uint64_t curt = get_current_ms();
        go_thread(uthread_mainB, NULL);
        go_sleep(2000); 
        printf("main sleep  wake i %u, %llu\n", i, get_current_ms());
    }

    return ;
}

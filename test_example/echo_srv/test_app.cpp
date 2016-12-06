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
#include <errno.h>
#include <string.h>

#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>



#include "util.h"
#include "uthread_api.h"

/*
 *一个简单的echo Srv
 */


/*
 * 开发者实现这个
 *
 */
void uthread_main(void *)
{
    printf("uthread main begin %llu\n",  get_current_ms());

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if( fd < 0 )
    {
        printf("socket error :%d\n", errno);
        return ;
    }

    int flags = fcntl(fd, F_GETFL,0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    
    struct sockaddr_in stAddr;
    memset(&stAddr, 0, sizeof(sockaddr_in));
    stAddr.sin_family = AF_INET;
    stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    stAddr.sin_port = htons(8888);

    if( bind(fd, (struct sockaddr*)&stAddr, sizeof(stAddr) ) < 0 )
    {
        printf("bind() func error %d\n", errno);
        close(fd);
        return ;
    }

    while(1)
    {
        struct sockaddr_in stCltAddr;
        socklen_t          addrLen; 

        char szBuf[1024];
        
        int rl = go_udprecv(fd,  szBuf, sizeof(szBuf), 0, (struct sockaddr*)&stCltAddr, &addrLen); 
        
        if (rl > 0 )
            int sl = go_udpsend(fd, szBuf, rl, 0, (struct sockaddr*)&stCltAddr, sizeof(stCltAddr));
    }

    close(fd);


    return ;
}

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
 *一个简单的echo  client
 */



void client_func(void *)
{
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
    stAddr.sin_addr.s_addr = inet_addr("10.66.91.181");
    stAddr.sin_port = htons(8888);

    char szBuf[100]={0};
    sprintf(szBuf, "my fd is %u\n", fd);
    
    int sl = go_udpsend(fd, szBuf, strlen(szBuf), 0, (struct sockaddr *)&stAddr, sizeof(stAddr)) ;
    if( sl < 0 )
    {
        printf(" fd:%d send %s errno:%d\n",fd, szBuf, errno ); 
        close(fd);
        return ;
    }

    struct sockaddr_in srvAddr;
    memset(&stAddr, 0, sizeof(sockaddr_in));
    socklen_t   srvAddrLen ;

    char rzBuf[100] = {0};
    int rl = go_udprecv(fd, rzBuf, 100, 20 , (struct sockaddr*) &srvAddr, &srvAddrLen);
    if (rl < 0  )
    {
        printf(" fd:%d recv %s errno:%d\n",fd, szBuf, errno ); 
        close(fd);
        return ;
    }

    printf("recv OK  fd%d recv:%s\n", fd, rzBuf);
    
    close(fd);

}

/*
 * 开发者实现这个
 *
 */
void uthread_main(void *)
{
    printf("uthread main begin %llu\n",  get_current_ms());


    uint64_t begin = get_current_ms();
    for(int i=0; i< 20 ; i++ ) 
    {
        go_thread(client_func, NULL); 
    }

    uint64_t end = get_current_ms();

    printf("start:%llu ,end :%llu\n", begin, end);


    return ;
}

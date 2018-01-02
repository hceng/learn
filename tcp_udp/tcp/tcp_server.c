/*
* tcp_server.c
# Copyright (C) 2017 hceng, <huangcheng.job@foxmail.com>
# Licensed under terms of GPLv2
#
# This program is used for TCP / UDP learning.
# https://hceng.cn/
*/
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>      
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define PORT_NUMBER 8888
#define BACKLOG     10

/* socket->bind->listen->accept->send/recv->close*/

int main(int argc, char **argv)
{
    int sock_fd, new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
    int ret;
    int addr_len;
    int recv_len;
	unsigned char recv_buf[1000];
    int client_num = -1;
    
    signal(SIGCHLD,SIG_IGN);
    
    /* socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);//AF_INET:IPV4;SOCK_STREAM:TCP
    if (-1 == sock_fd)
    {
        fprintf(stderr,"socket error:%s\n\a", strerror(errno));
        exit(1);
    }
    
    /* set sockaddr_in parameter*/
    memset(&server_addr, 0, sizeof(struct sockaddr_in));//clear
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY:This machine all IP
    server_addr.sin_port = htons(PORT_NUMBER);

    /* bind */
    ret = bind(sock_fd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr));
    if(-1 == ret)
    {
        fprintf(stderr,"bind error:%s\n\a", strerror(errno));
        close(sock_fd);
        exit(1);
    }
    
    /* listen */
    ret = listen(sock_fd, BACKLOG);
    if (-1 == ret)
    {
        fprintf(stderr,"listen error:%s\n\a", strerror(errno));
        close(sock_fd);
        exit(1);
    }
    
    /* accept */
    while(1)
    {
        addr_len = sizeof(struct sockaddr);
        new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (-1 == new_fd)
        {
            fprintf(stderr,"accept error:%s\n\a", strerror(errno));
            close(sock_fd);
            exit(1);
        }
        
        client_num++;
        fprintf(stderr, "Server get connetion form client%d: %s\n", client_num, inet_ntoa(client_addr.sin_addr));
        
        if (!fork())
		{
            /* Child process */
            while (1)
            {
                /* recv */
                recv_len = recv(new_fd, recv_buf, 999, 0);
                if (recv_len <= 0)
                {
                    fprintf(stderr, "recv error:%s\n\a", strerror(errno));
                    close(new_fd);	
                    exit(1);
                }
                else
                {
                    recv_buf[recv_len] = '\0';
                    printf("Get msg from client%d: %s\n", client_num, recv_buf);
                }
            }	
            close(new_fd);			
        }	   
    }
    
    /* close */
    close(sock_fd);
	exit(0); 
}
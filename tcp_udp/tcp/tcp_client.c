/*
* tcp_client.c
# Copyright (C) 2017 hceng, <huangcheng.job@foxmail.com>
# Licensed under terms of GPLv2
#
# This program is used for TCP / UDP learning.
# https://hceng.cn/
*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT_NUMBER 8888

/* socket->connect->send->close*/
int main(int argc, char *argv[])
{
    int sock_fd;
    struct sockaddr_in server_addr;
    struct hostent *host;
    int ret;
    unsigned char send_buf[1000];
	int send_len;
    
    if(argc != 2)
	{
		fprintf(stderr, "Usage:%s hostname\n\a", argv[0]);
		exit(1);
	}

	if((host = gethostbyname(argv[1])) == NULL)
	{
		fprintf(stderr,"gethostbyname error.\n");
		exit(1);
	}
    
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
    server_addr.sin_port = htons(PORT_NUMBER);
    ret = inet_aton(argv[1], &server_addr.sin_addr);
    if(0 == ret)
 	{
        fprintf(stderr,"server_ip error.\n");
        close(sock_fd);
		exit(1);
	}
    
    /* connect */
    ret = connect(sock_fd, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr));	
	if (-1 == ret)
	{
		fprintf(stderr,"connect error:%s\n\a", strerror(errno));
        close(sock_fd);
		exit(1);
	}
    
    while (1)
	{
		if (fgets(send_buf, 999, stdin))
		{
            /* send */
			send_len = send(sock_fd, send_buf, strlen(send_buf), 0);
			if (send_len <= 0)
			{
                fprintf(stderr,"send error:%s\n\a", strerror(errno));
				close(sock_fd);
				exit(1);
			}
		}
	}
    
    /* close */
    close(sock_fd);
    exit(0);
}
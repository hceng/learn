#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	int fd[2];
	pid_t pid;
	char buf[1024] = {0};
	int count = 0;

	if(pipe(fd) < 0)
	{
		perror("Unable to pipe");
		exit(1);
	}
#if 0
	pid = fork();
	if(pid < 0)
	{
		perror("Unable to fork");
		exit(1);
	}

	if(pid == 0)
	{
//		printf("fd[0] = %d , fd[1] = %d \n", fd[0], fd[1]);
		while(1)
		{
			read(fd[0], buf, 32);//如果管道中没有数据，读操作会阻塞
			if(strncmp(buf, "quit", 4) == 0)
				break;
			printf("--> %s \n", buf);
		}
	}
	else
	{
//		printf("fd[0] = %d , fd[1] = %d \n", fd[0], fd[1]);
		while(1)
		{
			fgets(buf, 32, stdin);
			write(fd[1], buf, strlen(buf) + 1);
			if(strncmp(buf, "quit", 4) == 0)
				break;
		}
		wait(NULL);
	}

#endif
#if 0
	while(1)
	{
		write(fd[1], buf, 1024);//如果缓冲区慢，则会阻塞在write函数
		count++;
		if(count == 64)
		{
			read(fd[0], buf, 1024);
			read(fd[0], buf, 1024);
			read(fd[0], buf, 1024);
			read(fd[0], buf, 1024);
		}
		printf("count = %d \n", count);
	}
#endif
	close(fd[0]);
	write(fd[1], buf, 1024);

	return 0;
}

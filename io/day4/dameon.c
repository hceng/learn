#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	pid_t pid;

	pid = fork();

	if(pid < 0)
	{
		perror("Unable to fork");
		exit(1);
	}

	if(pid == 0)
	{
		//创建新的会话，让子进程成为会话组的领导者，不受终端控制
		setsid();
		//改变当前目录为根目录
		chdir("/");
		//重设文件权限掩码
		umask(0);
		//关闭打开的文件描述符

		int i, fd_w;
		char buf[] = {"hello world\n"};

		//getdtablesize()返回这个进程的文件描述表的项数
		for(i = 0; i < getdtablesize(); i++)
			close(i);
		fd_w = open("/test123.txt", O_CREAT | O_WRONLY | O_APPEND, 0666);
		if(fd_w < 0)
		{
			perror("Unalbe to open");
			exit(1);
		}

		while(1)
		{
			write(fd_w, buf, 20);
			sleep(1);
		}

		close(fd_w);
	}
	else
	{
		exit(0);
	}
	return 0;
}

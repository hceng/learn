#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, const char *argv[])
{
	
	pid_t pid;
	pid = fork();

	if(pid < 0)
	{
		perror("Unable to fork");
		return -1;
	}

	if(pid == 0)
	{
		//子进程
		printf("child process parent %d ,child = %d\n",getppid(), getpid());
		exit(0);
	}
	else
	{
		//父进程:
		int status;
		sleep(4);
		wait(&status);
		printf("child exit\n");
		sleep(4);
		printf("parent status = %d\n", status);
	}
	return 0;
}

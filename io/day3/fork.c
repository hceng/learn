#include <unistd.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
	
	pid_t pid;

	printf("hello world\n");
	pid = fork();
	printf("pid = %d\n",pid);

	if(pid < 0)
	{
		perror("Unable to fork");
		return -1;
	}

	if(pid == 0)
	{
		//子进程
		printf("child process parent %d ,child = %d\n",getppid(), getpid());
	}
	else
	{
		//父进程:
		printf("parent process = %d, child = %d\n", getpid(), pid);
	}
	while(1);
	return 0;
}

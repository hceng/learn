#include <stdio.h>
#include <signal.h>
#include <stdlib.h>


pid_t pid;

void saler(int signo)
{
	if(signo == SIGINT)
	{
		kill(getppid(), SIGUSR1);
	}
	if(signo == SIGQUIT)
	{
		kill(getppid(), SIGUSR2);
	}
	if(signo == SIGUSR1)
	{
		printf("please get off the bus \n");
		exit(0);
	}
}

void driver(int signo)
{
	if(signo == SIGUSR1)
	{
		printf("let`s gogogo\n");
	}
	if(signo == SIGUSR2)
	{
		printf("stop the bus\n");
	}
	if(signo == SIGTSTP)
	{
		kill(pid, SIGUSR1);
		wait(NULL);
		exit(0);
	}
}

int main(int argc, const char *argv[])
{

	pid = fork();
	if(pid == 0)
	{
		//子进程
		signal(SIGINT, saler);
		signal(SIGQUIT, saler);
		signal(SIGUSR1, saler);
		signal(SIGTSTP, SIG_IGN);
		signal(SIGUSR2, SIG_IGN);

	}
	else
	{
		//父进程
		signal(SIGUSR1, driver);
		signal(SIGUSR2, driver);
		signal(SIGTSTP, driver);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
	}

		while(1)
		{
			
		}
	return 0;
}

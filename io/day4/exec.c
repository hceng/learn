#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>



int main(int argc, const char *argv[])
{
	printf("hello start \n");
#if 0
	if(execl("/bin/ls", "ls", "-l", "-a", NULL)	 < 0)
	{
		perror("Unable to execl");
		exit(1);
	}
#endif

#if 0
	if(execlp("ls", "ls", "-l", "-a", NULL)	 < 0)
	{
		perror("Unable to execl");
		exit(1);
	}
#endif
#if 1
	char *arg[] = {"ls", "-la", NULL};
	if(execv("/bin/ls", arg) < 0)
	{
		perror("Unable to execl");
		exit(1);
	}
#endif
	printf("hello end \n");
	return 0;
}

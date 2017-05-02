#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define N 32

int main(int argc, const char *argv[])
{
	int fd_r, fd_w;
	size_t bytes = 0;
	char buf[N] = {0};

	if(mkfifo("myfifo", 0666) < 0)
	{
		perror("Unable to mkfifo");
	//	exit(1);
	}
	fd_w = open("myfifo", O_WRONLY);
	if(fd_w < 0)
	{
		perror("Unable to open fd_w.");
		exit(1);
	}
	printf("fifo write\n");

	while(1)
	{
		fgets(buf, 32, stdin);
		write(fd_w, buf, strlen(buf) + 1);
		if(strncmp(buf, "quit", 4) == 0)
		break;
	}
	close(fd_w);
	
	return 0;
}	

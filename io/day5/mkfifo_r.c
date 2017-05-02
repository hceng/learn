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
	int fd_r;
	size_t bytes = 0;
	char buf[N] = {0};


	fd_r = open("myfifo", O_RDONLY);
	if(fd_r < 0)
	{
		perror("Unable to open fd_r");
		exit(1);
	}

	while(1)
	{
		read(fd_r, buf, 32);
		if(strncmp(buf, "quit", 4) == 0)
			break;
		printf("--> %s \n", buf);
	}
	close(fd_r);
	
	return 0;
}	

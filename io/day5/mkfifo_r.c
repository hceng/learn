#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

#define N 32

int main(int argc, const char *argv[])
{
	int fd_r, fd_w;
	size_t bytes = 0;
	char buf[N] = {0};
#if 0
	if(mkfifo("myfifo", 0666) < 0)
	{
		perror("Unable to mkfifo");
		//exit(1);
	}
#endif

	fd_w = open(argv[1], O_WRONLY);
	if(fd_w < 0)
	{
		perror("Unable to open");
		exit(1);
	}
	fd_r = open("myfifo", O_RDONLY);
	if(fd_r < 0)
	{
		perror("Unable to open fd_r");
		exit(1);
	}

	while((bytes = read(fd_r, buf, 32)) != 0)
	{
		write(fd_w, buf, bytes);
	}

	return 0;
}	

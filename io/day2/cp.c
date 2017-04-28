#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N 32

int main(int argc, const char *argv[])
{
	int fd_r, fd_w;
	ssize_t bytes = 0;
	char buf[N] = {0};

	fd_r = open(argv[1], O_RDONLY);
	if(fd_r < 0)
	{
		perror("Unable to open fd_r");
		return -1;
	}

	fd_w = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if(fd_w < 0)
	{
		perror("Unable to open fd_w");
		return -1;
	}

	while((bytes = read(fd_r, buf, 32)) != 0)
		write(fd_w, buf, bytes);

	close(fd_r);
	close(fd_w);
	return 0;
}

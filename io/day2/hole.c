#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, const char *argv[])
{
	int fd_w;
	char ch = 'a';

	fd_w = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if(fd_w < 0)
	{
		perror("Unable to open fd_w");
		return -1;
	}

	lseek(fd_w, 500, SEEK_SET);
	write(fd_w, &ch, 1);
	close(fd_w);

	return 0;
}

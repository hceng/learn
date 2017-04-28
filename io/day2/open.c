#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, const char *argv[])
{
	int fd;
	fd = open(argv[1], O_RDONLY | O_CREAT | O_EXCL, 0666);
	if(fd < 0)
	{
		perror("Unable to open");
		return -1;
	}

	printf("fd = %d \n", fd);

	close(fd);

	return 0;
}

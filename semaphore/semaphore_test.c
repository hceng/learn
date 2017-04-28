#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int fd;

	fd = open("/dev/semaphoretest", O_RDWR);
	
	if (fd < 0)
	{
		printf("app: can't open!\n");
	}
	else printf("app: open ok!\n");

	while(1);
	
	return 0;
}

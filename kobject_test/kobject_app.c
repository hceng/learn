
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

/* 
kobject_test 
*/
int fd;

int main(int argc, char **argv)
{
	fd = open("/dev/kobject_test", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
		return -1;
	}
	
	return 0;
}


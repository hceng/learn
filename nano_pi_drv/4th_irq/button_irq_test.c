
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* 
read button PA3
*/
int main(int argc, char **argv)
{
	int fd;
	unsigned int key_val = 0;
	unsigned int key_count = 0;
	
	fd = open("/dev/button", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}

	while(1)
	{
		read(fd, &key_val, 4);
		if(key_val == 1)
		{
			printf("key_count = %d. \n", key_count++);
		}
	}
 
	return 0;
}

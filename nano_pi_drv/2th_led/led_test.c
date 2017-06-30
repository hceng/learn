
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* 
led_test on
led_test off
*/
int main(int argc, char **argv)
{
	int fd;
	int val = 1;
	fd = open("/dev/led", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}
	if (argc != 2)
	{
		printf("Usage :\n");
		printf("%s <on|off>\n", argv[0]);
		return 0;
	}

	if (strcmp(argv[1], "on") == 0)
	{
		printf("on\n");
		val = 1;
	}
	else
	{
	    printf("off\n");

		val = 0;
	}
	
	write(fd, &val, 4);
 
	return 0;
}

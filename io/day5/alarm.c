#include <unistd.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
	int ret = -1;
	ret = alarm(9);
	printf("ret = %d \n", ret);
	sleep(3);
	ret = alarm(10);
	printf("ret = %d \n", ret);
	pause();

	printf("hello world \n");
	//while(1);
	return 0;
}

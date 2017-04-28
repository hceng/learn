#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	printf("hello world ");
#if 0 
	_exit(5);
#endif
	exit(5);
	printf("hello beijing \n");
	return 0;
}

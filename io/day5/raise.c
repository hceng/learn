#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	printf("hello wrold.\n");
	if(raise(atoi(argv[1])) < 0)
	{
		perror("Unable to raise");
		exit(1);
	}
	while(1);

	return 0;
}

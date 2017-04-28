#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
int main(int argc, const char *argv[])
{
	key_t key;

	key = ftok(".", 'a');
	if(key < 0)
	{
		perror("Unable to ftok");
		exit(1);
	}
	printf("key =  %#x \n", key);
	return 0;
}

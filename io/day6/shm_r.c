#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>

int main(int argc, const char *argv[])
{
	key_t key;
	int shmid;
	char * ptr;

	key = ftok(".", 'a');
	if(key < 0)
	{
		perror("Unable to ftok");
		exit(1);
	}

	shmid = shmget(key, 128, IPC_CREAT | IPC_EXCL | 0666);
	if(shmid < 0)
	{
		if(errno == EEXIST)
		{
			shmid = shmget(key, 128, 0666);
		}
		else
		{
			perror("Unable to shmget");
			exit(1);
		}
	}
	if((ptr = shmat(shmid, NULL, 0)) == (char *)-1)
	{
		perror("Unable to shmat");
		exit(1);
	}
	printf("--> %s \n",ptr);
	if(shmdt(ptr) < 0)
	{
		perror("Unable to shmdt");
		exit(1);
	}
	if(shmctl(shmid, IPC_RMID, NULL) < 0)
	{
		perror("Unable to shmctl");
		exit(1);
	}
	return 0;
}

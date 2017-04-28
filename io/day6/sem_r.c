#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

union semun
{
	int val;
};

int main(int argc, const char *argv[])
{
	key_t key;
	int semid;
	union semun  mysem;
	struct sembuf mybuf;

	key = ftok(".", 'a');
	if(key < 0)
	{
		perror("fail to ftok");
		exit(1);
	}
	semid = semget(key, 2, IPC_CREAT | IPC_EXCL | 0666);
	if(semid < 0)
	{
		if(errno == EEXIST)
			semid = semget(key, 2, 0666);
		else
		{
			perror("fail to semget");
			exit(1);
		}
	}
	else
	{
		mysem.val = 0;
		semctl(semid, 0, SETVAL, mysem);
		mysem.val = 1;
		semctl(semid, 1, SETVAL, mysem);
	}
	int shmid = shmget(key, 128, IPC_CREAT | 0666);
	if(shmid < 0)
	{
		if(errno == EEXIST)
		{
			shmid = shmget(key, 128, 0666);
		}
		else{
			perror("fail to shmget");
			exit(1);
		}
	}
	char *buf = (char *)shmat(shmid, NULL, 0);
	if(buf == (char *)-1)
	{
		perror("fail to shmat");
		exit(1);
	}
	while(1)
	{
		mybuf.sem_num = 0;
		mybuf.sem_op = -1;
		mybuf.sem_flg = 0;
		semop(semid, &mybuf, 1);
		printf("--> %s \n", buf);
		mybuf.sem_num = 1;
		mybuf.sem_op = 1;
		mybuf.sem_flg = IPC_NOWAIT;
		semop(semid, &mybuf, 1);
	}
	system("ipcs -s");
	return 0;
}


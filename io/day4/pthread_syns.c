#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

#define N 32

char buf[N] = {0};
sem_t mysem;

void *pthread_fun(void * arg)
{
	while(1)
	{
		sem_wait(&mysem);
		printf("--> %s\n",buf);
	}
}

int main(int argc, const char *argv[])
{
	pthread_t tid;
	sem_init(&mysem, 0, 0);
	if(pthread_create(&tid, NULL, pthread_fun, NULL) != 0)
	{
		perror("Unable to pthread_create");
		exit(1);
	}
	while(1)
	{
		fgets(buf, 32, stdin);
		sem_post(&mysem);
	}
	return 0;
}

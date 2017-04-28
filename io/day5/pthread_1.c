#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
sem_t mysem1;
sem_t mysem2;

pthread_mutex_t mutex;
pthread_cond_t mycond;

int value_1, value_2;

void *pthread_fun(void * arg)
{
	while(1)
	{
		sem_wait(&mysem1);
		if(value_1 == value_2)
		{
			printf("value_1 = %d , value_2 = %d \n", value_1, value_2);
		}
		sem_post(&mysem2);
	}
}

int main(int argc, const char *argv[])
{
	pthread_t tid;
	int count = 0;

	pthread_mutex_init(&mutex, NULL);
	sem_init(&mysem1, 0, 0);
	sem_init(&mysem2, 0, 1);
	if(pthread_create(&tid, NULL, pthread_fun, NULL) != 0)
	{
		perror("Unable to pthread_create");
		exit(1);
	}
	while(1)
	{
		count++;
		sem_wait(&mysem2);
		value_1 = count;
		value_2 = count;
		sem_post(&mysem1);
	//	sleep(1);
	}

	pthread_join(tid, NULL);
	return 0;
}


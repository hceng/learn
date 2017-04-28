#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex;

int value_1, value_2;

void *pthread_fun(void * arg)
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if(value_1 == value_2)
		{
			printf("value_1 = %d , value_2 = %d \n", value_1, value_2);
		}
		pthread_mutex_unlock(&mutex);
	}
}

int main(int argc, const char *argv[])
{
	pthread_t tid;
	int count = 0;

	pthread_mutex_init(&mutex, NULL);

	if(pthread_create(&tid, NULL, pthread_fun, NULL) != 0)
	{
		perror("Unable to pthread_create");
		exit(1);
	}
	while(1)
	{
		count++;
		pthread_mutex_lock(&mutex);
		value_1 = count;
		value_2 = count;
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}

	pthread_join(tid, NULL);
	return 0;
}


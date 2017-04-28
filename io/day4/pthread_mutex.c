#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

pthread_mutex_t lock;
int value_1, value_2;

void *pthread_fun(void * arg)
{
	while(1)
	{
		pthread_mutex_lock(&lock);
		if(value_1 == value_2)
		{
			printf("value_1 = %d, value_2 = %d\n", value_1, value_2);
		}
		pthread_mutex_unlock(&lock);
	}
}

int main(int argc, const char *argv[])
{
	int count = 0;
	pthread_t pthread;
	
	if(pthread_mutex_init(&lock, NULL) < 0)
	{
		perror("fail to pthread_mutex_init ");
		exit(1);
	}
	if(pthread_create(&pthread, NULL, pthread_fun, NULL) < 0)
	{
		perror("fail to pthread_create ");
		exit(1);
	}
	while(1)
	{
		count++;
		pthread_mutex_lock(&lock);
		value_1 = count;
		value_2 = count;
		pthread_mutex_unlock(&lock);
	}
	pthread_join(pthread, NULL);

	return 0;
}

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void *pthread_fun(void * arg)
{
	int i = 20;
	printf("*arg = %d \n", *(int *)arg);
	while(i--)
	{
		printf("pthread i = %d \n", i);
		sleep(1);
		if(i == 10)
			pthread_exit("pthread exit");
	}
}

int main(int argc, const char *argv[])
{
	pthread_t tid;
	int num = 18;

	if(pthread_create(&tid, NULL, pthread_fun, &num) != 0)
	{
		perror("Unable to pthread_create");
		exit(1);
	}
	printf("pthread start \n");
	void * ret;
	pthread_join(tid, &ret);
	//while(1);
	printf("pthread end  %s \n", (char *)ret);
	return 0;
}


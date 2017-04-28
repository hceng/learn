#include <stdio.h>

int main(int argc, const char *argv[])
{
	FILE * fp;
	int count = 0;
	fclose(stdin);
	while((fp = fopen(argv[1], "r")) != NULL)
			count++;
	
	printf("count = %d \n", count);

	return 0;
}

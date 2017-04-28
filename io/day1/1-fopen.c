#include <stdio.h>

int main(int argc, const char *argv[])
{
	FILE * fp;
	unsigned int i = 0;

	fp = fopen(argv[1], "w");
	if(fp == NULL)
	{
		printf("Unable to fopen \n");
		return -1;
	}
/*	printf("argc = %d\n",argc);
	for(i; i<argc; i++)
	{
		printf("argv[%d]=%s\n",i,argv[i]);
	}
*/	printf("read success \n");

	fclose(fp);
	return 0;
}

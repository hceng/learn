#include <stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fp;

	fp = fopen(argv[1], "r+");
	if(fp == NULL)
	{
		fprintf(stdout,"Unable to fopen \n");
		return -1;
	}

	fseek(fp, 0, SEEK_SET);
	fgetc(fp);
	fgetc(fp);
	long pos = ftell(fp);
	printf("pos = %ld \n", pos);

	rewind(fp);
	pos = ftell(fp);
	printf("pos = %ld \n", pos);
	return 0;
}

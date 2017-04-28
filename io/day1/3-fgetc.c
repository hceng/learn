#include <stdio.h>

int main(int argc, const char *argv[])
{
	FILE * fp;
	
	fp = fopen(argv[1], "r");
	if(fp == NULL)
	{
		printf("Unable to fopen\n");
		return -1;
	}

	char ch = fgetc(fp);
	 ch = fgetc(fp);
	 ch = fgetc(fp);

	//printf("ch = %c \n", ch);

	fputc(ch, stdout);
	putchar(10);
	fclose(fp);
	return 0;
}

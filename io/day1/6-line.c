#include <stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fp_r;
	char ch;
	int line = 0;

	fp_r = fopen(argv[1], "r");
	if(fp_r == NULL)
	{
		fprintf(stdout, "Unable to fopen \n");
		return -1;
	}
	
	while((ch = fgetc(fp_r)) != EOF)
	{
		if(ch == '\n')
			line++;
	}

	printf("line = %d \n",line);
	fclose(fp_r);

	return 0;
}

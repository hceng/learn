#include <stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fp_r;
	char ch;

	fp_r = fopen(argv[1], "r");
	if(fp_r == NULL)
	{
		//	printf("Unable to fopen \n");
		fprintf(stdout, "Unable to fopen \n");
		return -1;
	}
	
	while((ch = fgetc(fp_r)) != EOF)
	{
		//printf("%c",ch);
		fputc(ch, stdout);
	}
	fclose(fp_r);
	return 0;
}

#include <stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fp_r, *fp_w;
	char ch;

	fp_r = fopen(argv[1], "r");
	if(fp_r == NULL)
	{
		fprintf(stdout,"Unable to fopen fp_r\n");
		return -1;
	}

	fp_w = fopen(argv[2], "w");
	if(fp_w == NULL)
	{
		fprintf(stdout, "Unable to fopen fp_w \n");
	}

	while((ch = fgetc(fp_r)) != EOF)
	{
		fputc(ch, fp_w);
	}


	fclose(fp_r);
	fclose(fp_w);
	return 0;
}

#include <stdio.h>

#define N 32

int main(int argc, const char *argv[])
{
	FILE *fp_r;
	char buf[N] = {0};

	fp_r = fopen(argv[1], "r");
	if(fp_r == NULL)
	{
		fprintf(stdout, "Unable to fopen fp_r \n");
		return -1;
	}

	fgets(buf, 32, fp_r);

	fputs(buf, stdout);

	fclose(fp_r);
	return 0;
}

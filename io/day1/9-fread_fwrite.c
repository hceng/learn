#include <stdio.h>

#define N 32

int main(int argc, const char *argv[])
{
	FILE *fp_r, *fp_w;
	char buf[N] = {0};

	fp_r = fopen(argv[1], "r");
	if(fp_r == NULL)
	{
		fprintf(stdout, "Unable to fopen fp_r\n");
		return -1;
	}

	fp_w = fopen(argv[2], "w");
	if(fp_w == NULL)
	{
		fprintf(stdout, "Unable to fopen fp_w\n");
		return -1;
	}
	size_t bytes = fread(buf, 5, 3, fp_r);
	fwrite(buf, 5, 2, fp_w);

	//printf("bytes = %d \n",bytes);
	return 0;
}

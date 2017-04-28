#include <stdio.h>
#include <time.h>

int main(int argc, const char *argv[])
{
	FILE *fp;
	char ch;
	time_t myt;
	struct tm *mytm;
	int line = 0;

	//打开文件
	fp = fopen(argv[1], "a+");
	if(fp == NULL)
	{
		fprintf(stderr, "Unable to fopen \n");
		return -1;
	}

	//获取行数
	while((ch = fgetc(fp)) != EOF)
	{
		if(ch == '\n')
		{
			line++;
		}
	}

	while(1)
	{
		time(&myt);
		mytm = localtime(&myt);
		fprintf(fp, "%d, %d-%d-%d %d: %d : %d  \n",++line, mytm->tm_year + 1900, mytm->tm_mon + 1, mytm->tm_mday, 
																mytm->tm_hour, mytm->tm_min, mytm->tm_sec);
		fflush(fp);
		sleep(1);
	}


	return 0;
}


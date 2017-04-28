/*************************************************************************
	> File Name: hceng_time.c
	> Author:hceng
	> Mail: huangcheng.job@foxmail.com 
	> Created Time: Wed 26 Apr 2017 06:48:22 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	FILE *fd;
	char ch;
	unsigned int line = 0;
	time_t *get_time;
	struct tm *local_time;

	//add data to file
	fd = fopen(argv[1], "a+");
	if(fd == NULL)
	{
		fprintf(stderr,"can't open file!");
		return -1;
	}
    
	//get line
	while((ch = fgetc(fd)) != EOF)
	{
		if(ch == '\n')
			++line;
	}
	//show
	while(1)
	{
		time(get_time);
		local_time = localtime(get_time);
		fprintf(fd,"%d. %d-%d-%d  %d:%d:%d\n",++line,local_time->tm_year+1900,local_time->tm_mon,local_time->tm_mday,local_time->tm_hour,local_time->tm_min,local_time->tm_sec);
		printf("%d. %d-%d-%d  %d:%d:%d\n",line,local_time->tm_year+1900,local_time->tm_mon,local_time->tm_mday,local_time->tm_hour,local_time->tm_min,local_time->tm_sec);
		fflush(fd);
		sleep(1);
	}
	
	fclose(fd);
	return 0;
}

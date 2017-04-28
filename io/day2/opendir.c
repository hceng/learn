#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, const char *argv[])
{
	DIR * dir;
	struct dirent *myread;

	dir = opendir(argv[1]);
	if(dir == NULL)
	{
		perror("Unable to opendir");
		return -1;
	}
#if 0
	myread = readdir(dir);

	printf("%s \n", myread->d_name);
#endif

	while((myread= readdir(dir)) != NULL)
	{
		if(myread->d_name[0] != '.')
			printf("%s \n", myread->d_name);
	}
	closedir(dir);
	return 0;
}

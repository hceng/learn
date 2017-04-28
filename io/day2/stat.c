#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, const char *argv[])
{
	struct stat mystat;

	if(stat(argv[1], &mystat) < 0)
	{
		perror("unable to stat");
		return -1;
	}

	switch(S_IFMT & mystat.st_mode)
	{
		case S_IFSOCK:
			putchar('r');
			break;
		case S_IFLNK:
			putchar('l');
			break;
		case S_IFREG:
			putchar('-');
			break;
		case S_IFBLK:
			putchar('b');
			break;
		case S_IFDIR:
			putchar('d');
			break;
		case S_IFCHR:
			putchar('c');
			break;
		case S_IFIFO:
			putchar('p');
			break;
	}
	if(S_IRWXU & mystat.st_mode)
	{
		if(S_IRUSR & mystat.st_mode)
		{
			putchar('r');
		}
		else
		{
			putchar('-');
		}
		if(S_IWUSR & mystat.st_mode)
		{
			putchar('w');
		}
		else
		{
			putchar('-');
		}
		if(S_IXUSR & mystat.st_mode)
		{
			putchar('x');
		}
		else
		{
			putchar('-');
		}
	}
	if(S_IRWXG & mystat.st_mode)
	{
		if(S_IRGRP & mystat.st_mode)
		{
			putchar('r');
		}
		else
		{
			putchar('-');
		}
		if(S_IWGRP & mystat.st_mode)
		{
			putchar('w');
		}
		else
		{
			putchar('-');
		}
		if(S_IGRP & mystat.st_mode)
		{
			putchar('x');
		}
		else
		{
			putchar('-');
		}
	}
	if(S_IRWXO & mystat.st_mode)
	{
		if(S_IROTH & mystat.st_mode)
		{
			putchar('r');
		}
		else
		{
			putchar('-');
		}
		if(S_IWOTH & mystat.st_mode)
		{
			putchar('w');
		}
		else
		{
			putchar('-');
		}
		if(S_IXOTH & mystat.st_mode)
		{
			putchar('x');
		}
		else
		{
			putchar('-');
		}
	}
	return 0;
}


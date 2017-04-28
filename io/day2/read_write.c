#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define N 32

int main(int argc, const char *argv[])
{
	char buf[N] = {0};
	while(1)
	{
		ssize_t bytes = read(0, buf, 16);

		write(1, buf, bytes);
	}
	return 0;
}

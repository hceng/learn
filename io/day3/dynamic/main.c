#include <stdio.h>

int main(int argc, const char *argv[])
{
	int x = 8, y = 9;

	printf("x + y = %d \nx - y = %d \n", add(x, y), sub(x, y));
	return 0;
}

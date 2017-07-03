
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>


/*
PA2：led
PL11: IRQ button
button control led
*/

int fd_led;
int fd_button;

int led_val = 0;

void button_signal_fun(int signum)
{
	unsigned int key_val;
	
	read(fd_button, &key_val, 4);
	if(key_val == 1)
	{
		led_val = !led_val;
		write(fd_led, &led_val, 4);
	}
}

int main(int argc, char **argv)
{

	int Oflags;

	signal(SIGIO, button_signal_fun);
	
	fd_led = open("/dev/led", O_RDWR);
	if (fd_led < 0)
	{
		printf("can't open led!\n");
		return 0;
	}
	
	fd_button = open("/dev/button", O_RDWR);
	if (fd_button < 0)
	{
		printf("can't open button!\n");
		return 0;
	}

	fcntl(fd_button, F_SETOWN, getpid());
	
	Oflags = fcntl(fd_button, F_GETFL); 
	
	fcntl(fd_button, F_SETFL, Oflags | FASYNC);

	
	while(1)
	{
		sleep(1000);
	}
 
	return 0;
}

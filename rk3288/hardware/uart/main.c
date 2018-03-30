
#include "led.h"
#include "uart.h"

int raise (int sig_nr)
{
	return 0;
}		

int main(void)
{
	led_mode(3);
	
	uart_init();
	
	while(1)
	{
		printf_test();
	
		delay(10000000);
	}
					
	return 0;
}



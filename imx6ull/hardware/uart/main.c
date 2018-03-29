
#include "uart.h"
#include "printf.h"
#include "led.h"


int  main()
{
	char a = '4';
	char b = '5';

	uart_init();

	uart_PutChar(a);
	delay(1000000);
	
	uart_PutChar(b);
	delay(1000000);	
			
	return 0;
}



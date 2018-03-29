
#include "uart.h"
#include "printf.h"
#include "led.h"


int  main()
{
	char a = '4';
	char b = '5';
	uart_init();
	
#if 0
	unsigned int f = 0;
	unsigned char a1 = 0;
	unsigned char a2 = 0;
	unsigned char a3 = 0;
	unsigned char a4 = 0;

	f = get_uart_clk();
	a1 = (unsigned char)((f & 0xFF000000)>>24);
	a2 = (unsigned char)((f & 0x00FF0000)>>16);
	a3 = (unsigned char)((f & 0x000000FF)>>8);
	a4 = (unsigned char)(f & 0x000000FF);
	
	uart_PutChar(a1);
	uart_PutChar(a2);
	uart_PutChar(a3);
	uart_PutChar(a4);
#endif

	uart_PutChar(a);
	delay(1000000);
	
	uart_PutChar(b);
	delay(1000000);	
			
	return 0;
}



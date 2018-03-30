
#include "led.h"
#include "uart.h"

int main(int addr)
{
	unsigned char a1 = 0;
	unsigned char a2 = 0;
	unsigned char a3 = 0;
	unsigned char a4 = 0;

	unsigned char a = '0';
	unsigned char b = 'x';
	unsigned char c = 'a';
	unsigned char d = 'b';
	
	char x[] = "x0x";  
	
	char y[] = "0x";
	
	a1 = (unsigned char)((addr & 0xFF000000)>>24);
	a2 = (unsigned char)((addr & 0x00FF0000)>>16);
	a3 = (unsigned char)((addr & 0x000000FF)>>8);
	a4 = (unsigned char)(addr & 0x000000FF);

	uart_init();
	
	led_mode(3);
	while(1)
	{
		//rk_uart_sendbyte(a);
		//rk_uart_sendbyte(b);
		//rk_uart_sendbyte(c);
		//rk_uart_sendbyte(a1);
		//rk_uart_sendbyte(a2);
		//rk_uart_sendbyte(a3);
		//rk_uart_sendbyte(a4);
		
		rk_uart_sendstring(x);
		rk_uart_sendstring(y);
		
		
		//rk_uart_sendhex(addr);
		
		//rk_uart_sendstring(x1);
		//rk_uart_sendstring(y1);
		
		delay(1000000);
	}
					
	return 0;
}



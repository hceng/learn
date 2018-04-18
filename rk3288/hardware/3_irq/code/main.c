
#include "led.h"
#include "int.h"
#include "key.h"
#include "timer.h"
#include "uart.h"
#include "printf.h"

void irq_test_printf(void)
{
	printf("irq_test_printf!\n\r");
}

int main(void)
{
	led_mode(3);
	
	uart_init();
	printf("eMMC boot!\n\r");
	
	gic_irq_init();
	printf("interrupt init ok!\n\r");
	
	key_test_interrupt(); 
	
	timer_test_interrupt(); 
	
	while(1)
	{
		
	}
	
	return 0;
}
	

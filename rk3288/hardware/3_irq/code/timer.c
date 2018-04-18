/*************
Function£ºKEY
GPIO7_A3
Default -> High level
Push    -> Low level
**************/ 

#include "timer.h"
#include "printf.h"
#include "int.h"

void delay(unsigned int  i)
{
	volatile unsigned int count = i;
	while(count--);
}

	
//Use the interrupt method
static void clear_timer_irq(void)
{
	REG4 = 0x01;
}

static void key_irq_isr(void)
{
	clear_timer_irq();
	printf("=================timer irq isr===========.\n\r");	
}


static void timer_init(void)
{
    REG1 = 0x00FFFFFF;
    REG2 = 0x00FFFFFF;
    REG3 = 0x05;
}

void timer_test_interrupt(void)
{
    timer_init();
	register_irq(TIMER_6CH_3, key_irq_isr);
	irq_handler_enable(TIMER_6CH_3);
}


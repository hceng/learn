/*************
Function：KEY
GPIO7_A3
Default -> High level
Push    -> Low level
**************/ 

#include "key.h"
#include "timer.h"
#include "printf.h"
#include "int.h"

#if 0
static void debug_printf(unsigned int i)
{
	printf("==================%d=======================\n\r", i);
	printf("GPIO7_INTEN         = %d\n\r", GPIO7_INTEN         );
	printf("GPIO7_INTMASK       = %d\n\r", GPIO7_INTMASK       );
	printf("GPIO7_INTTYPE_LEVEL = %d\n\r", GPIO7_INTTYPE_LEVEL );
	printf("GPIO7_INT_POLARITY  = %d\n\r", GPIO7_INT_POLARITY  );
	printf("GPIO7_INT_STATUS    = %d\n\r", GPIO7_INT_STATUS    );
	printf("GPIO7_INT_RAWSTATUS = %d\n\r", GPIO7_INT_RAWSTATUS );
	printf("GPIO7_DEBOUNCE      = %d\n\r", GPIO7_DEBOUNCE      );
	printf("GPIO7_PORTA_EOI     = %d\n\r", GPIO7_PORTA_EOI     );
	printf("GPIO7_EXT_PORTA     = %d\n\r", GPIO7_EXT_PORTA     );
}
#endif

//Use the inquire method
static void set_gpio_input(void)
{
	GPIO7_SWPORTA_DDR &= ~(0x01<<3); //0: Input (default)			
}

static int get_key_value(void)
{
	set_gpio_input();
	return (GPIO7_EXT_PORTA & (0x01<<3)) ? 1: 0;	
}

void key_test_inquire(void)
{
	if(1 != get_key_value())
		printf("Key Push!\n\r");		
}
		
	
//Use the interrupt method
static void clear_key_irq(void)
{
	GPIO7_PORTA_EOI |=  (0x01<<3); //1: Clear interrupt
}

static void key_irq_isr(void)
{	
	clear_key_irq();
	
	if (GPIO7_EXT_PORTA & (0x01<<3))
		printf("==================key irq: release!===============\n\r");
	else
		printf("==================key irq: push!==================\n\r");//实测:按下无问题，松开时产生大量中断？？？
}

static void set_gpio_irq(void)
{
	GPIO7_SWPORTA_DDR   &= ~(0x01<<3); //0: Input (default)	
	GPIO7_INTEN         |=  (0x01<<3); //1: Configure Port A bit as interrupt 
	GPIO7_INTTYPE_LEVEL |=  (0x01<<3); //1: Edge-sensitive
	GPIO7_INT_POLARITY  &= ~(0x01<<3); //0: Active-low (default)
	GPIO7_DEBOUNCE      |=  (0x01<<3); //1: Enable debounce	
}

void key_test_interrupt(void)
{
    set_gpio_irq();
	register_irq(IRQ_GPIO7, key_irq_isr);
    gic_irq_set_trig(IRQ_GPIO7, 0);
	irq_handler_enable(IRQ_GPIO7);
}





/*************
Function：Timer
**************/ 

#include "timer.h"
#include "printf.h"
#include "int.h"

//timer0 is used for delay.
//实测6us
void delay_us(volatile unsigned long int  i)
{
	unsigned long int count_value = 24*i; //24MHz; 1s=1/[(10^6)*24MHz]
	
	timer0->controlreg &= ~(0x01<<0); //P1571 Timer enable: 0:disable
	
	timer0->controlreg |=  (0x01<<1); //Timer mode:1:user-defined count mode
	timer0->controlreg &= ~(0x01<<2); //Timer interrupt mask: 0:mask
	
	timer0->load_count0 = count_value & 0xFFFFFFFF; 
	//timer0->load_count1//如果要传入64位数,需要两个参数来表示,反而复杂了,没必要;
	
	timer0->controlreg |=  (0x01<<0); //Timer enable

	while(!(timer0->intstatus & (0x01<<0)));
	
	timer0->intstatus |= (0x01<<0);//Write 1 clear the interrupt	
	
	timer0->controlreg &= ~(0x01<<0); //Timer enable: 0:disable
}

//实测1.0086ms
void delay_ms(volatile unsigned long int i)
{
	for(; i>0; i--)
		delay_us(1000);
}

void delay_s(volatile unsigned long int i)
{
	for(; i>0; i--)
		delay_ms(1000);
}


//timer1 is used for interrupt test.
static void clear_timer_irq(void)
{
	timer1->intstatus |= (0x01<<0);
}

static void key_irq_isr(void)
{
	clear_timer_irq();
	printf("===========timer irq===========\n\r");	
}

static void timer_init(void)
{
	timer1->controlreg &= ~(0x01<<0); //Timer disable
	
	timer1->controlreg &= ~(0x01<<1); //Timer mode:0:free-running mode
	timer1->controlreg |=  (0x01<<2); //Timer interrupt mask: 1:not mask
	
    timer1->load_count0 = 0x016E3600; //24000000=1s
	
	timer1->controlreg |=  (0x01<<0); //Timer enable
	
}

void timer_test_interrupt(void)
{
    timer_init();
	register_irq(TIMER_6CH_1, key_irq_isr);
	irq_handler_enable(TIMER_6CH_1);
}







/*


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

*/
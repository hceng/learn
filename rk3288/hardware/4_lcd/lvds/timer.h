#ifndef   __TIMER_H__
#define   __TIMER_H__

#define    TIMER0_5_BASE             0xFF6B0000                                      //P37
#define    TIMER6_7_BASE             0xFF810000                                      //P37

#define    TIMER0_5(n)               (TIMER0_5_BASE + n * 0x20)  //5>=n>=0
#define    TIMER6_7(n)               (TIMER6_7_BASE + n * 0x20)  //1>=n>=0

typedef volatile struct timer_reg { //P1569
	unsigned long load_count0;		//0x00 Timer(0~5) Load Count Register     Low 32 bits
	unsigned long load_count1;		//0x04 Timer(0~5) Load Count Register     High 32 bits 
	unsigned long current_value0;	//0x08 Timer(0~5) Current Value Register  Low 32 bits
	unsigned long current_value1;	//0x0c Timer(0~5) Current Value Register  High 32 bits 
	unsigned long controlreg;		//0x10 Timer(0~5) Control Register
	unsigned long reserved0[1];     //0x14  
	unsigned long intstatus;		//0x18 Timer(0~5) Interrupt Status Register 
} timer_reg, *p_timer;

#define timer0       ((p_timer)TIMER0_5(0))
#define timer1       ((p_timer)TIMER0_5(1))
#define timer2       ((p_timer)TIMER0_5(2))
#define timer3       ((p_timer)TIMER0_5(3))
#define timer4       ((p_timer)TIMER0_5(4))
#define timer5       ((p_timer)TIMER0_5(5))

#define timer6       ((p_timer)TIMER6_7(0))
#define timer7       ((p_timer)TIMER6_7(1))

extern void delay_us(volatile unsigned long int  i);
extern void delay_ms(volatile unsigned long int  i);
extern void delay_s(volatile unsigned long int i);

extern void timer_test_interrupt(void);

#endif
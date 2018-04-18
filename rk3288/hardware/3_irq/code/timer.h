#ifndef   __TIMER_H__
#define   __TIMER_H__

#define    REG1  (*(volatile unsigned long *)( 0xFF6B0000 + 0x20 * 3 + 0x08))      
#define    REG2  (*(volatile unsigned long *)( 0xFF6B0000 + 0x20 * 3 + 0x00))  
#define    REG3  (*(volatile unsigned long *)( 0xFF6B0000 + 0x20 * 3 + 0x10))  
#define    REG4  (*(volatile unsigned long *)( 0xFF6B0000 + 0x20 * 3 + 0x18))  

extern void delay(unsigned int  i);
extern void timer_test_interrupt(void);

#endif
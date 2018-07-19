#ifndef   __KEY_H__
#define   __KEY_H__

#define    GPIO7               0xFF7E0000                                   //P37
#define    GPIO7_SWPORTA_DR    (*(volatile unsigned long *)(GPIO7 + 0x00))  //1580 Port A data register 
#define    GPIO7_SWPORTA_DDR   (*(volatile unsigned long *)(GPIO7 + 0x04))  //1580 Port A data direction register 
                             
#define    GPIO7_INTEN         (*(volatile unsigned long *)(GPIO7 + 0x30))  //1580 Interrupt enable register
#define    GPIO7_INTMASK       (*(volatile unsigned long *)(GPIO7 + 0x34))  //1580 Interrupt mask register 
#define    GPIO7_INTTYPE_LEVEL (*(volatile unsigned long *)(GPIO7 + 0x38))  //1580 Interrupt level register
#define    GPIO7_INT_POLARITY  (*(volatile unsigned long *)(GPIO7 + 0x3C))  //1580 Interrupt polarity register 
#define    GPIO7_INT_STATUS    (*(volatile unsigned long *)(GPIO7 + 0x40))  //1580 Interrupt status of port A 
#define    GPIO7_INT_RAWSTATUS (*(volatile unsigned long *)(GPIO7 + 0x44))  //1580 Raw Interrupt status of port A 
#define    GPIO7_DEBOUNCE      (*(volatile unsigned long *)(GPIO7 + 0x48))  //1580 Debounce enable register 
#define    GPIO7_PORTA_EOI     (*(volatile unsigned long *)(GPIO7 + 0x4C))  //1580 Port A clear interrupt register 
#define    GPIO7_EXT_PORTA     (*(volatile unsigned long *)(GPIO7 + 0x50))  //1580 Port A external port register 

extern void key_test_inquire(void);
extern void key_test_interrupt(void);

#endif
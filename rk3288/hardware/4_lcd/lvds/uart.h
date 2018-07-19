
#ifndef   __UART_H__
#define   __UART_H__

#define    GRF                  0xFF770000//P37 
#define    UART_DBG             0xFF690000//P37 

#define    GRF_GPIO7CH_IOMUX    (*(volatile unsigned long *)(GRF + 0x0078))  //P241

#define    UART_SRR             (*(volatile unsigned long *)(UART_DBG + 0x0000))  //P1630 Software Reset Register
#define    UART_THR             (*(volatile unsigned long *)(UART_DBG + 0x0000))  //P1630 Transmit Holding Register
#define    UART_DLL             (*(volatile unsigned long *)(UART_DBG + 0x0000))  //P1630 Divisor Latch (Low)
#define    UART_DLH             (*(volatile unsigned long *)(UART_DBG + 0x0004))  //P1630 Divisor Latch (High)
#define    UART_IER             (*(volatile unsigned long *)(UART_DBG + 0x0004))  //P1630 Interrupt Enable Register 
#define    UART_LCR             (*(volatile unsigned long *)(UART_DBG + 0x000C))  //P1630 Line Control Register
#define    UART_MCR             (*(volatile unsigned long *)(UART_DBG + 0x0010))  //P1630 Modem Control Register
#define    UART_USR             (*(volatile unsigned long *)(UART_DBG + 0x007C))  //P1630 UART Status Register
#define    UART_SFE             (*(volatile unsigned long *)(UART_DBG + 0x0098))  //P1631 Shadow FIFO Enable
#define    UART_SRT             (*(volatile unsigned long *)(UART_DBG + 0x009C))  //P1631 Shadow RCVR Trigger
#define    UART_STET            (*(volatile unsigned long *)(UART_DBG + 0x00A0))  //P1631 Shadow TX Empty Trigger


void  uart_init(void);
void rk_uart_sendbyte(unsigned char byte);
void rk_uart_sendstring(char *ptr);
void rk_uart_sendhex(unsigned int val);

#endif


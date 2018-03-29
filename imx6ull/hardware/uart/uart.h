
#ifndef   __UART_H__
#define   __UART_H__

void raise (int sig_nr);
extern void  uart_init();
extern void  uart_PutChar(char  c);

extern void  uart_PutString(char *ptr);
//extern void printHex(unsigned int val);

unsigned int get_uart_clk(void);

#endif


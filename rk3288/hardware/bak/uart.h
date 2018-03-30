
#ifndef   __UART_H__
#define   __UART_H__

void  uart_init(void);
void rk_uart_sendbyte(unsigned char byte);
void rk_uart_sendstring(char *ptr);
void rk_uart_sendhex(unsigned int val);

#endif


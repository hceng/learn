#include "s3c24xx.h"
#include "serial.h"

#define	TXD0READY	(1<<2)
#define	RXD0READY	(1)

#define PCLK            50000000    // init.c¿¿clock_init¿¿¿¿PCLK¿50MHz
#define UART_CLK        PCLK        //  UART0¿¿¿¿¿¿PCLK
#define UART_BAUD_RATE  115200      // ¿¿¿
#define UART_BRD        ((UART_CLK  / (UART_BAUD_RATE * 16)) - 1)

/*
 * ¿¿¿UART0
 * 115200,8N1,¿¿¿
 */
void uart0_init(void)
{
    GPHCON  |= 0xa0;    // GPH2,GPH3¿¿TXD0,RXD0
    GPHUP   = 0x0c;     // GPH2,GPH3¿¿¿¿

    ULCON0  = 0x03;     // 8N1(8¿¿¿¿¿¿¿¿¿1¿¿¿¿)
    UCON0   = 0x05;     // ¿¿¿¿¿UART¿¿¿¿PCLK
    UFCON0  = 0x00;     // ¿¿¿FIFO
    UMCON0  = 0x00;     // ¿¿¿¿¿
    UBRDIV0 = UART_BRD; // ¿¿¿¿115200
}

void putc(unsigned char c)
{
	while( ! (UTRSTAT0 & TXD0READY) );
	UTXH0 = c;
}

unsigned char getc(void)
{
#ifdef SERIAL_ECHO
	unsigned char ret;
#endif	
	while( ! (UTRSTAT0 & RXD0READY) );
	ret = URXH0;
#ifdef SERIAL_ECHO	
	if (ret == 0x0d || ret == 0x0a)
	{
		putc(0x0d);
		putc(0x0a);
	}
	else
	{
		putc(ret);		
	}
#endif	
	return ret;
}


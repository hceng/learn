
#include "led.h"

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

              
//rk3288_u-boot\drivers\serial\serial_rk.c                   

static void rk_uart_iomux(void)
{
	//uart引脚复用
	GRF_GPIO7CH_IOMUX = (0x03<<28) | (0x03<<24) | (0x01<<12) | (0x01<<8); //P268  rk3288_u-boot\arch\arm\cpu\armv7\rk32xx\iomux-rk3288.c
}

static void rk_uart_reset()
{
	/* UART reset, rx fifo & tx fifo reset */
	UART_SRR = (0x01<<0) | (0x01<<1) | (0x01<<2);
	/* interrupt disable */
	UART_IER = 0x00;
}

static void rk_uart_set_iop(void)
{
	UART_MCR = 0x00;
}

static  void rk_uart_set_lcr(void)
{
	UART_LCR &= ~(0x03<<0);
	UART_LCR |=  (0x03<<0); //8bits
	
	UART_LCR &= ~(0x01<<3); //parity disabled

	UART_LCR &= ~(0x01<<2);	//1 stop bit
}

static void rk_uart_set_baudrate(void)
{
	volatile unsigned long rate;
	unsigned long baudrate = 115200;
	
	/* uart rate is div for 24M input clock */
	//rate = 24000000 / 16 / baudrate;
	rate = 13;

	UART_LCR |= (0x01<<7);

	UART_DLL = (rate & 0xFF);
	UART_DLH = ((rate >> 8) & 0xFF);
	
	UART_LCR &= ~(0x01<<7);
}

static void rk_uart_set_fifo(void)
{
	/* shadow FIFO enable */
	UART_SFE = 0x01;
	/* fifo 2 less than */
	UART_SRT = 0x03;
	/* 2 char in tx fifo */
	UART_STET = 0x01;
}

void uart_init(void)
{
	rk_uart_iomux();
	rk_uart_reset();
	
	rk_uart_set_iop();
	rk_uart_set_lcr();
	
	rk_uart_set_baudrate();
	
	rk_uart_set_fifo();
}

void rk_uart_sendbyte(unsigned char byte)
{
	while((UART_USR & (0x01<<1)) == 0);

	UART_THR = byte;
}


void rk_uart_sendstring(char *ptr)
{
	while(*ptr)   
		rk_uart_sendbyte(*ptr++);
}


/* 0xABCDEF12 */

void rk_uart_sendhex(unsigned int val)
{
	int i;
	unsigned int arr[8];

	/* 先取出每一位的值 */
	for (i = 0; i < 8; i++)
	{
		arr[i] = val & 0xf;
		val >>= 4;   /* arr[0] = 2, arr[1] = 1, arr[2] = 0xF */
	}

	/* 打印 */
	rk_uart_sendstring("0x");
	for (i = 7; i >=0; i--)
	{
		if (arr[i] >= 0 && arr[i] <= 9)
			rk_uart_sendbyte(arr[i] + '0');
		else if(arr[i] >= 0xA && arr[i] <= 0xF)
			rk_uart_sendbyte(arr[i] - 0xA + 'A');
	}
}






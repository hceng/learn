
#include "led.h"
#include "uart.h"

//rk3288_u-boot\drivers\serial\serial_rk.c                   
static void rk_uart_iomux(void)
{
	//uartÒý½Å¸´ÓÃ
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


void rk_uart_sendhex(unsigned int val)
{
	int i;
	unsigned int arr[8];

	for (i = 0; i < 8; i++)
	{
		arr[i] = val & 0xf;
		val >>= 4;   /* arr[0] = 2, arr[1] = 1, arr[2] = 0xF */
	}

	rk_uart_sendstring("0x");
	for (i = 7; i >=0; i--)
	{
		if (arr[i] >= 0 && arr[i] <= 9)
			rk_uart_sendbyte(arr[i] + '0');
		else if(arr[i] >= 0xA && arr[i] <= 0xF)
			rk_uart_sendbyte(arr[i] - 0xA + 'A');
	}
}






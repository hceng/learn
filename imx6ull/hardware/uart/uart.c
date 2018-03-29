
#include "led.h"

#define    CCM_CCGR5            (*(volatile unsigned long *)0x020C407C)  //P706  CCM Clock Gating Register 5 
#define    CCM_CSCDR1           (*(volatile unsigned long *)0x020C4024)  //P658  CCM Serial Clock Divider Register 1
                                
#define    CCM_ANALOG_PLL_USB1  (*(volatile unsigned long *)0x020C8010)  //P710  Analog USB1 480MHz PLL Control Register 
#define    CCM_CACRR            (*(volatile unsigned long *)0x020C4010)  //P658  CCM Arm Clock Root Register
                                
                                
#define    PAD_UART1_TX         (*(volatile unsigned long *)0x020E0310)  //P1550 SW_PAD_CTL_PAD_UART1_TX_DATA SW PAD Control Register
#define    PAD_UART1_RX         (*(volatile unsigned long *)0x020E0314)  //P1550 SW_PAD_CTL_PAD_UART1_RX_DATA SW PAD Control Register
                                
#define    IOMUXC_UART1_TX      (*(volatile unsigned long *)0x020E0084)  //P1543 SW_MUX_CTL_PAD_UART1_TX_DATA SW MUX Control Register
#define    IOMUXC_UART1_RX      (*(volatile unsigned long *)0x020E0088)  //P1543 SW_MUX_CTL_PAD_UART1_RX_DATA SW MUX Control Register
                                
                                
#define    UART1_URXD           (*(volatile unsigned long *)0x02020000)  //P3609 UART Receiver Register
#define    UART1_UTXD           (*(volatile unsigned long *)0x02020040)  //P3609 UART Transmitter Register
#define    UART1_UCR1           (*(volatile unsigned long *)0x02020080)  //P3609 UART Control Register 1
#define    UART1_UCR2           (*(volatile unsigned long *)0x02020084)  //P3609 UART Control Register 2
#define    UART1_UCR3           (*(volatile unsigned long *)0x02020088)  //P3609 UART Control Register 3
#define    UART1_UCR4           (*(volatile unsigned long *)0x0202008C)  //P3609 UART Control Register 4
#define    UART1_UFCR           (*(volatile unsigned long *)0x02020090)  //P3609 UART FIFO Control Register
#define    UART1_USR1           (*(volatile unsigned long *)0x02020094)  //P3609 UART Status Register 1
#define    UART1_USR2           (*(volatile unsigned long *)0x02020098)  //P3609 UART Status Register 2
#define    UART1_UESC           (*(volatile unsigned long *)0x0202009C)  //P3609 UART Escape Character Register
#define    UART1_UTIM           (*(volatile unsigned long *)0x020200A0)  //P3609 UART Escape Timer Register
#define    UART1_UBIR           (*(volatile unsigned long *)0x020200A4)  //P3609 UART BRM Incremental Register
#define    UART1_UBMR           (*(volatile unsigned long *)0x020200A8)  //P3610 UART BRM Modulator Register
#define    UART1_UBRC           (*(volatile unsigned long *)0x020200AC)  //P3610 UART UART Baud Rate Count Register
#define    UART1_ONEMS          (*(volatile unsigned long *)0x020200B0)  //P3610 UART UART One Millisecond Register
#define    UART1_UTS            (*(volatile unsigned long *)0x020200B4)  //P3610 ART Test Register 
#define    UART1_UMCR           (*(volatile unsigned long *)0x020200B8)  //P3610 UART RS-485 Mode Control Register



static void uart1_clock_enable(void)
{
	//uart时钟
	CCM_CSCDR1 |= (0x01<<6); //P676 Selector for the UART clock multiplexor:1 derive clock from osc_clk
	CCM_CCGR5  |= (0x03<<24); //uart1 clock (uart1_clk_enable)
}

static void uart1_iomux(void)
{
	//uart引脚复用
	IOMUXC_UART1_TX |=  (0x01<<16 | 0x02<<14 | 0x01<<13 | 0x01<<12 | 0x02<<6 | 0x06<<3 | 0x01<<0);
	IOMUXC_UART1_RX |=  (0x01<<16 | 0x02<<14 | 0x01<<13 | 0x01<<12 | 0x02<<6 | 0x06<<3 | 0x01<<0);
 
	IOMUXC_UART1_TX &= ~(0x0F<<0); //P1578  0000 ALT0 — Select mux mode: ALT0 mux port: UART1_TX of instance: uart1
	IOMUXC_UART1_RX &= ~(0x0F<<0); //P1579  0000 ALT0 — Select mux mode: ALT0 mux port: UART1_RX of instance: uart1
}

void raise (int sig_nr)
{
	;
}

unsigned int get_uart_clk(void)
{
	unsigned int reg, uart_podf;
	unsigned int freq, div;

	//div = CCM_ANALOG_PLL_USB1;
	div = CCM_CACRR;
	div &= 0x00000003;
	freq = 26000000 * (20 + (div << 1));	

	reg = CCM_CSCDR1;
	if (reg & (1<<6))
			freq = 26000000;
	

	reg &= 0x3F; 
	uart_podf = reg >> 0;

	return freq / (uart_podf + 1);
}

//uart配置
static void uart1_config(void)
{
	unsigned int clk;
	
	UART1_UCR1 = 0;
	UART1_UCR2 = 0;
	while(!(UART1_UCR2 & (1<<0)));
	UART1_UCR3 = (0x704 | (1<<7));
	UART1_UCR4 = (0x8000);
	UART1_UESC = (0x2b);
	UART1_UTIM = (0);
	UART1_UTS  = (0);	
	
	clk = get_uart_clk();
	UART1_UFCR = (4<<7 | 2<<10 | 1<<0);
	//UART1_UFCR = (4<<7);
	UART1_UBIR = (0xf);
	UART1_UBMR = (clk / (2 * 125000));//115200 - 9.42     125000 - 8.75
	
	UART1_UCR2 = (1<<5 | 1<<14 | 1<<1 | 1<<2 | 1<<0);
	UART1_UCR1 = (1<<0);
}

void  uart_init()
{
	uart1_clock_enable();

	uart1_iomux();

	uart1_config();
}

void  uart_PutChar(char  c)
{
	UART1_UTXD = c;	
	while(!(UART1_UTS & (1<<6)));
}


void uart_PutString(char *ptr)
{
	 
	while(*ptr != '\0')
	{          
		uart_PutChar(*ptr++);
	}
}


/* 0xABCDEF12 */
/*
void printHex(unsigned int val)
{
	int i;
	unsigned int arr[8];
	char x[] = "x0x";

	///先取出每一位的值 
	for (i = 0; i < 8; i++)
	{
		arr[i] = val & 0xf;
		val >>= 4;   //arr[0] = 2, arr[1] = 1, arr[2] = 0xF 
	}

	// 打印 
	uart_PutString(x);
	for (i = 7; i >=0; i--)
	{
		if (arr[i] >= 0 && arr[i] <= 9)
			uart_PutChar(arr[i] + '0');
		else if(arr[i] >= 0xA && arr[i] <= 0xF)
			uart_PutChar(arr[i] - 0xA + 'A');
	}
}
*/






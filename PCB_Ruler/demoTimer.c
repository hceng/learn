/*
定时器计算公式：
假设Clock为32kHz,PRE=0;
TIME =(FFFFFFFF - TLDR + 1)*[ 1 /(32000 + PS)]
单位：s
本程序：(FFFFFFFF-FFF82FF + 1)*(1 / 32001) = 1s
*/

#include "AM437X_Core.h"
#include "led.h"

#define DMTIMER_WRITE_POST_TLDR              (0x04)
#define DMTIMER_WRITE_POST_TCRR              (0x02)
#define DMTIMER_WRITE_POST_TCLR              (0x01)

unsigned int timerCount = 0xFFFF82FF;
unsigned int timerFlag = 0;

int main(void)
{
	//停止定时器;
	DMTimer2WaitForWrite(DMTIMER_WRITE_POST_TCLR);
	DMTIMER2->TCLR &= ~(0x01<<0);;

	//开启自动加载模式(AR);关闭比较模式(CE);关闭分频器PRE=0;
	DMTimer2WaitForWrite(DMTIMER_WRITE_POST_TCLR);
	DMTIMER2->TCLR &= ((0x01<<1) | ~(0x01<<6) | ~(0x01<<5));
	
	//设置定时器计数初值;
	DMTimer2WaitForWrite(DMTIMER_WRITE_POST_TLDR);
    DMTIMER2->TLDR = 0xFFFF82FF;

	//启动定时器;
	DMTimer2WaitForWrite(DMTIMER_WRITE_POST_TCLR);
	DMTIMER2->TCLR &= (0x01<<0);;
	
	while(1)
	{
		//检查中断标志位以判断定时器是否溢出;
		if(DMTIMER2->IRQSTS == (0x01<<1))
		{
			//清除标志位;
			DMTIMER2->IRQSTS |= (0x01<<1);
			
			if(timerFlag)
			{
				led_on();
				timerFlag = 0;
			}
			else
			{
				led_off();
				timerFlag = 1;
			}
		}
	}	
}

//确保寄存器reg写完成
DMTimer2WaitForWrite(unsigned char reg)
{
	if(DMTIMER2->TSICR & (0x01<<2))
		while(reg & DMTIMER2->TWPS);
}


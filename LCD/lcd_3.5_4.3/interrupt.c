#include "s3c24xx.h"

//extern void I2CIntHandle(void);

void (*isr_handle_array[50])(void);

void Dummy_isr(void)
{
    while(1);
}

void init_irq(void)
{
    int i = 0;
    for (i = 0; i < sizeof(isr_handle_array) / sizeof(isr_handle_array[0]); i++)
    {
        isr_handle_array[i] = Dummy_isr;
    }

    INTMOD = 0x0;	      // 所有中断都设为IRQ模式
    INTMSK = BIT_ALLMSK;  // 先屏蔽所有中断

//	isr_handle_array[ISR_IIC_OFT]  = I2CIntHandle;
}

void IRQ_Handle(void)
{
	unsigned long oft = INTOFFSET;
    
	//清中断
	if (oft == 4)
        EINTPEND = 1<<7;    //EINT4-7合用IRQ4，注意EINTPEND[3:0]保留未用，向这些位写入1可能导致未知结果
	SRCPND = 1<<oft;	
	INTPND = INTPND;	 

    /* 调用中断服务程序 */
    isr_handle_array[oft]();
}


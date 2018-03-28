/*************
¹¦ÄÜ£ºLEDS
D12 - SNVS_TAMPER1 - GPIO5_IO1
D13 - SNVS_TAMPER2 - GPIO5_IO2
**************/ 

//#define    CCM_CCGR1         (*(volatile unsigned long *)0x020C406C)  //P659 
//#define    IOMUXC_TAMPER1    (*(volatile unsigned long *)0x0229000C)  //P1495 MUX_MODE
//#define    IOMUXC_TAMPER2    (*(volatile unsigned long *)0x02290010)  //P1497 MUX_MODE
#define    GPIO5_DR          (*(volatile unsigned long *)0x020AC000)  //P1357 GPIO data register
#define    GPIO5_GDIR        (*(volatile unsigned long *)0x020AC004)  //P1357 GPIO direction register

void  delay(unsigned int  i)
{
	volatile unsigned int count = i;
	while(count--);
}

void  led_on()
{
	//IOMUXC_TAMPER1 = (0x05<<0);//P1500 MUX_MODE  101 ALT5
	//IOMUXC_TAMPER2 = (0x05<<0);
	
	GPIO5_GDIR   |= (0x01<<1 | 0x01<<2); //P1359  1 OUTPUT ¡ª GPIO is configured as output
	
	while(1)
	{			
		GPIO5_DR |=  (0x01<<1);
		GPIO5_DR &= ~(0x01<<2);
		delay(500000);

		GPIO5_DR &= ~(0x01<<1);
		GPIO5_DR |=  (0x01<<2);
		delay(500000);
	}
}






	
	

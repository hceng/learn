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

#define    DDR_ADDRESS       (*(volatile unsigned long *)0x80000000)  //P175 ARM Platform Memory Map
#define    DDR_ADDR(offset)  (*(volatile unsigned long *)(0x80000000+offset))  

#define    MMDC_MDCTL        (*(volatile unsigned long *)0x021B0000)  //P2259 MMDC Core Control Register

void delay(unsigned int  i)
{
	volatile unsigned int count = i;
	while(count--);
}



/*********************
mode   led1   led2
0      0      0
1      1      0
2      0      1
3      1      1

4      0_1    0
5      0      0_1
6      0_1    0_1 
**********************/

void led_mode(int mode)
{	
	if(0 == mode) 
	{
		GPIO5_GDIR &= ~(0x01<<1 | 0x01<<2); 
	}
	else if(1 == mode) 
	{
		//GPIO5_GDIR &= ~(0x01<<1 | 0x01<<2); 
		
		GPIO5_GDIR |=  (0x01<<1); 
		GPIO5_DR   &= ~(0x01<<1);
	}
	else if(2 == mode) 
	{
		//GPIO5_GDIR &= ~(0x01<<1 | 0x01<<2); 
		
		GPIO5_GDIR |=  (0x01<<2); 
		GPIO5_DR   &= ~(0x01<<2);
	}
	else if(3 == mode) 
	{
		GPIO5_GDIR |=  (0x01<<1 | 0x01<<2); 
		GPIO5_DR   &= ~(0x01<<1 | 0x01<<2);
	}
	
	else if(4 == mode) 
	{
		GPIO5_GDIR &= ~(0x01<<1 | 0x01<<2); 
		
		GPIO5_GDIR   |= (0x01<<1); 
		
		
		while(1)
		{			
			GPIO5_DR |=  (0x01<<1);
			delay(100000);

			GPIO5_DR &= ~(0x01<<1);
			delay(100000);
		}
	}
	else if(5 == mode) 
	{
		GPIO5_GDIR &= ~(0x01<<1 | 0x01<<2); 
		GPIO5_GDIR   |= (0x01<<2); 
		while(1)
		{			
			GPIO5_DR &= ~(0x01<<2);
			delay(100000);
			
			GPIO5_DR |=  (0x01<<2);
			delay(100000);
		}
	}	
	else if(6 == mode) 
	{
		GPIO5_GDIR   |= (0x01<<1 | 0x01<<2); //P1359  1 OUTPUT ¡ª GPIO is configured as output
		while(1)
		{			
			GPIO5_DR |=  (0x01<<1);
			GPIO5_DR &= ~(0x01<<2);
			delay(100000);

			GPIO5_DR &= ~(0x01<<1);
			GPIO5_DR |=  (0x01<<2);
			delay(100000);
		}
	}
}

#define    TEST_SIZE   (1024*1024)

void test_ddr(void)
{
	int i;
	unsigned int offset;
	int equal_flag = 1;

	//Ð´¼Ä´æÆ÷
	offset = 0;
	for(i=0; i<TEST_SIZE; i++)
	{
		DDR_ADDR(offset) = i;
		offset = offset + 4;
	}

	//¶Á¼Ä´æÆ÷²¢ÅÐ¶Ï
	offset = 0;
	for(i=0; i<TEST_SIZE; i++)
	{
		if( i != DDR_ADDR(offset) ) 
		{
			equal_flag = 0;
			break;
		}
		offset = offset + 4;
	}
	
	//ÏÔÊ¾
	if(DDR_ADDR(0) == 1)  
		led_mode(1);
	else if(DDR_ADDR(4*8) == 8)
		led_mode(3);
	else 
		led_mode(2);
	
	/*
	if(equal_flag)
		led_mode(3);	
	else
		led_mode(2);
	*/
	
	
}

void read_ddr_reg(void)
{
	unsigned int reg_value = 0;
	
	reg_value = MMDC_MDCTL;
	
	if (reg_value & (0x01<<30)) //SDE_1
		led_mode(1);	

	
	if (reg_value & (0x01<<31)) //SDE_0
		led_mode(2);//½á¹ûÁÁ
	
	while(1);
}






	
	

/*************
Function：LEDS
working led - blue   - GPIO8_A1 - Low  level on/High level off
power led   - yellow - GPIO8_A2 - Low  level on/High level off
**************/ 
#define    GPIO8             0xFF7F0000                                     //P37
#define    GPIO_SWPORTA_DR   (*(volatile unsigned long *)(GPIO8 + 0x04*0))  //1580 Port A data register 
#define    GPIO_SWPORTA_DDR  (*(volatile unsigned long *)(GPIO8 + 0x04*1))  //1580 Port A data direction register 

#define    DDR_ADDRESS       (*(volatile unsigned long *)0x00000000)        //P37 
#define    DDR_ADDR(offset)  (*(volatile unsigned long *)(0x00000000+offset))  


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
		GPIO_SWPORTA_DDR &= ~(0x01<<1 | 0x01<<2); 
	}
	else if(1 == mode) 
	{
		GPIO_SWPORTA_DDR &= ~(0x01<<1 | 0x01<<2); 
		
		GPIO_SWPORTA_DDR |=  (0x01<<1); 
		GPIO_SWPORTA_DR  &= ~(0x01<<1);
	}
	else if(2 == mode) 
	{
		GPIO_SWPORTA_DDR &= ~(0x01<<1 | 0x01<<2); 
		
		GPIO_SWPORTA_DDR |=  (0x01<<2); 
		GPIO_SWPORTA_DR  &= ~(0x01<<2);
	}
	else if(3 == mode) 
	{
		GPIO_SWPORTA_DDR |=  (0x01<<1 | 0x01<<2); 
		GPIO_SWPORTA_DR  &= ~(0x01<<1 | 0x01<<2);
	}
	
	else if(4 == mode) 
	{
		GPIO_SWPORTA_DDR &= ~(0x01<<1 | 0x01<<2); 
		
		GPIO_SWPORTA_DDR   |= (0x01<<1); 
		
		
		while(1)
		{			
			GPIO_SWPORTA_DR |=  (0x01<<1);
			delay(1000000);

			GPIO_SWPORTA_DR &= ~(0x01<<1);
			delay(1000000);
		}
	}
	else if(5 == mode) 
	{
		GPIO_SWPORTA_DDR &= ~(0x01<<1 | 0x01<<2); 
		GPIO_SWPORTA_DDR   |= (0x01<<2); 
		while(1)
		{			
			GPIO_SWPORTA_DR &= ~(0x01<<2);
			delay(1000000);
			
			GPIO_SWPORTA_DR |=  (0x01<<2);
			delay(1000000);
		}
	}	
	else if(6 == mode) 
	{
		GPIO_SWPORTA_DDR   |= (0x01<<1 | 0x01<<2); 
		while(1)
		{			
			GPIO_SWPORTA_DR |=  (0x01<<1);
			GPIO_SWPORTA_DR &= ~(0x01<<2);
			delay(1000000);

			GPIO_SWPORTA_DR &= ~(0x01<<1);
			GPIO_SWPORTA_DR |=  (0x01<<2);
			delay(1000000);
		}
	}
}

#define    TEST_SIZE   (1024*1024)

void test_ddr(void)
{
	int i;
	unsigned int offset;
	int equal_flag = 1;

	//写寄存器
	offset = 0x30000; //查看反汇编，DDR前面部分(0开始)有数据，所以不能从0开始写测
	for(i=0; i<TEST_SIZE; i++)
	{
		DDR_ADDR(offset) = i;
		offset = offset + 4;
	}

	//读寄存器并判断
	offset = 0x30000;
	for(i=0; i<TEST_SIZE; i++)
	{
		if( i != DDR_ADDR(offset) ) 
		{
			equal_flag = 0;
			break;
		}
		offset = offset + 4;
	}
	
	//显示
	if(DDR_ADDR(0x30000+0) == 1)  
		led_mode(1);
	else if(DDR_ADDR(0x30000+4*8) == 8)
		led_mode(3);//实测亮，证明DDR初始化过了
	else 
		led_mode(2);
	
	/*
	if(equal_flag)
		led_mode(3);	
	else
		led_mode(2);
	*/
	
	
}





	
	

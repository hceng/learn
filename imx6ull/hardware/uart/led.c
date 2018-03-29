/*************
¹¦ÄÜ£ºLEDS
D12 - SNVS_TAMPER1 - GPIO5_IO1
D13 - SNVS_TAMPER2 - GPIO5_IO2
**************/ 
#define    GPIO5_DR          (*(volatile unsigned long *)0x020AC000)  //P1357 GPIO data register
#define    GPIO5_GDIR        (*(volatile unsigned long *)0x020AC004)  //P1357 GPIO direction register

#define    DDR_ADDRESS       (*(volatile unsigned long *)0x80000000)  //P175 ARM Platform Memory Map

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
		GPIO5_GDIR &= ~(0x01<<1 | 0x01<<2); 
		
		GPIO5_GDIR |=  (0x01<<1); 
		GPIO5_DR   &= ~(0x01<<1);
	}
	else if(2 == mode) 
	{
		GPIO5_GDIR &= ~(0x01<<1 | 0x01<<2); 
		
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
		GPIO5_GDIR   |= (0x01<<1 | 0x01<<2); 
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





	
	

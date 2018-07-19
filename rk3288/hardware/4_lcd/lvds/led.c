/*************
Function£ºLEDS
working led - blue   - GPIO8_A1 - Low  level on/High level off
power led   - yellow - GPIO8_A2 - Low  level on/High level off
**************/ 

#include "led.h"
#include "timer.h"

/*********************
mode   led1   led2
0      0      0
1      1      0
2      0      1
3      1      1

4      0_1    0_1 
**********************/

void led_mode(int mode)
{	
	if(0 == mode) 
	{
		GPIO8_SWPORTA_DDR &= ~(0x01<<1 | 0x01<<2); 
	}
	else if(1 == mode) 
	{
		GPIO8_SWPORTA_DDR &= ~(0x01<<1 | 0x01<<2); 
		
		GPIO8_SWPORTA_DDR |=  (0x01<<1); 
		GPIO8_SWPORTA_DR  &= ~(0x01<<1);
	}
	else if(2 == mode) 
	{
		GPIO8_SWPORTA_DDR &= ~(0x01<<1 | 0x01<<2); 
		
		GPIO8_SWPORTA_DDR |=  (0x01<<2); 
		GPIO8_SWPORTA_DR  &= ~(0x01<<2);
	}
	else if(3 == mode) 
	{
		GPIO8_SWPORTA_DDR |=  (0x01<<1 | 0x01<<2); 
		GPIO8_SWPORTA_DR  &= ~(0x01<<1 | 0x01<<2);
	}
	else if(4 == mode) 
	{
		GPIO8_SWPORTA_DDR   |= (0x01<<1 | 0x01<<2); 
		while(1)
		{			
			GPIO8_SWPORTA_DR |=  (0x01<<1);
			GPIO8_SWPORTA_DR &= ~(0x01<<2);
			//delay(1000000);

			GPIO8_SWPORTA_DR &= ~(0x01<<1);
			GPIO8_SWPORTA_DR |=  (0x01<<2);
			//delay(1000000);
		}
	}
}






	
	

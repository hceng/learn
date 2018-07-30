/*************
Function£ºLEDS
SDIO0_WP/GPIO4_D3_d
**************/ 

#include "led.h"
#include "timer.h"

/*********************
mode   led1  
0      0     
1      1     
2      0_1     
**********************/

void led_mode(int mode)
{	
	GPIO4_SWPORTA_DDR &= ~(0x01<<27); 

	if(0 == mode) 
	{
		GPIO4_SWPORTA_DR  &= ~(0x01<<27);
	}
	else if(1 == mode) 
	{
		GPIO4_SWPORTA_DR |=  (0x01<<27); 
	}
	else if(2 == mode) 
	{
		while(1)
		{			
			GPIO4_SWPORTA_DR  &= ~(0x01<<27);
			delay_s(1);

			GPIO4_SWPORTA_DR  |=  (0x01<<27); 
			delay_s(1);
		}
	}
}






	
	

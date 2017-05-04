
#include "s3c24xx.h"

void delay(void)
{
	volatile unsigned int i = 1000000;
	while(i--);
	
}

void IRQ_Handle()
{
    unsigned long oft = INTOFFSET;
    
    switch( oft )
    {

        case 0: 
        {   
            delay();
            oft = INTOFFSET;
            if(oft == 0)
            {
                puts("led1.\n\r");
                GPFDAT |= (0x7<<4);   
                GPFDAT &= ~(1<<4); 
            }   
            break;
        }
        
  
        case 2:
        {   
            delay();
            oft = INTOFFSET;
            if(oft == 2)
            {
          		puts("led2.\n\r");
                GPFDAT |= (0x7<<4);   
                GPFDAT &= ~(1<<5); 
            } 
            break;
        }

      
        case 5:
        {   
            delay();
            oft = INTOFFSET;
            if(oft == 5)
            {
    			puts("led3.\n\r");
                GPFDAT |= (0x7<<4);   
                GPFDAT &= ~(1<<6);  
            }                
            break;
        }

        default:
            break;
    }

    if( oft == 5 ) 
        EINTPEND = (1<<11);     
    SRCPND = 1<<oft;
    INTPND = 1<<oft;
}

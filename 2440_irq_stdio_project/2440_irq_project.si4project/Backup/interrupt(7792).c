
#include "s3c24xx.h"

void IRQ_Handle()
{
    unsigned long oft = INTOFFSET;
    unsigned long val;
    
    switch( oft )
    {
        case 0: 
        {   
            GPFDAT |= (0x7<<4);   
            GPFDAT &= ~(1<<4);     
            break;
        }
        
  
        case 2:
        {   
            GPFDAT |= (0x7<<4);   
            GPFDAT &= ~(1<<5);  
            break;
        }

      
        case 5:
        {   
            GPFDAT |= (0x7<<4);   
            GPFDAT &= ~(1<<6);                  
            break;
        }

        default:
            break;
    }

 
    EINTPEND = (1<<11);   
    SRCPND = 1<<oft;
    INTPND = 1<<oft;
}

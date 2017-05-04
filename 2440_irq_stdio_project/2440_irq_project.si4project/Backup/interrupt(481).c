
#include "s3c24xx.h"

/*判断是哪种中断*/
void IRQ_Handle()
{
    unsigned long oft = INTOFFSET;
    
    switch( oft )
    {
        case 0: 
        {   
        	puts("led1\n\r");
            GPFDAT |= (0x7<<4);   // 所有LED熄灭
            GPFDAT &= ~(1<<4);      // LED1点亮
            break;
        }
        
        // S3被按下
        case 2:
        {   
        	puts("led2\n\r");
            GPFDAT |= (0x7<<4);   // 所有LED熄灭
            GPFDAT &= ~(1<<5);      // LED2点亮
            break;
        }

        // K4被按下
        case 5:
        {   
        	puts("led4s\n\r");
            GPFDAT |= (0x7<<4);   // 所有LED熄灭
            GPFDAT &= ~(1<<6);      // LED4点亮                
            break;
        }

        default:
            break;
    }

    //清中断
    if( oft == 5 ) 
        EINTPEND = (1<<11);   // EINT8_23合用IRQ5
    SRCPND = 1<<oft;
    INTPND = 1<<oft;
}


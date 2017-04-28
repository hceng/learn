#include <stdio.h>
#include "serial.h"
#include "lcdlib.h"
#include "s3c24xx.h"

int main()
{
    char c;
    
    uart0_init();   // 波特率115200，8N1(8个数据位，无校验位，1个停止位)
    
    while (1)
    {
        printf("\r\n##### Test TFT LCD #####\r\n");
        printf("[1] TFT240320 8Bit\n\r");
        printf("[2] TFT240320 16Bit\n\r");
        printf("[3] TFT480272 8Bit\n\r");
        printf("[4] TFT480272 16Bit\n\r");
		printf("[5] TFT480272 16Bit hceng\n\r");
        printf("Enter your selection: ");

        c = getc();
        printf("%c\n\r", c);
        switch (c)
        {
            case '1':
            {
                Test_Lcd_Tft_8Bit_240320();
                break;
            }
            
            case '2':
            {
                Test_Lcd_Tft_16Bit_240320();
                break;
            }

            case '3':
            {
                Test_Lcd_Tft_8Bit_480272();
                break;
            }
            
            case '4':
            {
                Test_Lcd_Tft_16Bit_480272();
                break;
            }
			case '5':
            {
                Test_Lcd_Tft_16Bit_480272_hceng();
                break;
            }
            
            default: 
                break;
        }
    }
    
    return 0;
}

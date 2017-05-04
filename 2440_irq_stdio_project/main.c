
#include "stdio.h"
#include "serial.h"


int main(void)
{
	int i = 0;
	
	char str[100];
		
  uart0_init( );	//波特率57600，8N1(8个数据位，无校验位，1个停止位)

	//printf("\n\rStart : \n\r");
	printf("\n\rok!! \n\r");
	
	while(1)
	{
		//printf("Enter a value : \n\r");
		//scanf("%s", str);
		//sscanf(str,"%d", &i);
		//printf("print interger : %d, 0x%x\n\r", i, i);
	}

	return 0;
}

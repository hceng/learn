#include <stdio.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <linux/input.h>  

int main()  
{  
	int keys_fd;      
	char ret[2];   
	struct input_event t;  

	keys_fd = open("/dev/input/event0", O_RDONLY);    //打开TP设备  
	if(keys_fd<=0){  
		printf("open /dev/input/event0 device error!\n");  
		return 0;  
	}  

	while(1)  
	{     
		if(read(keys_fd, &t, sizeof(t))==sizeof(t)) 
        {  
			if (t.type == EV_KEY)
            {  
				printf("  type: EV_KEY, event = %s, value = %d \r\n",   
						t.code == BTN_TOUCH ? "BTN_TOUCH" : "Unkown", t.value);   
			}  
			else if(t.type == EV_ABS)
            {  
				printf("  type: EV_ABS, event = %s, value = %d \r\n",   
						t.code == ABS_X ? "ABS_X" :   
						t.code == ABS_Y ? "ABS_Y" :   
						t.code == ABS_PRESSURE ? "ABS_PRESSURE" :"Unkown", t.value); 
			}  
		}  
	}     
	close(keys_fd);  

	return 0;  
} 

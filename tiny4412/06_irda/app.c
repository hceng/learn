#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <linux/input.h>  
  
int main()  
{  
    int fd;  
    int version;  
    int ret;  
    struct input_event ev;  
      
    fd = open("/dev/input/event0", O_RDONLY);  
    if (fd < 0) {  
        printf("open file failed\n");  
        exit(1);  
    }  
  
    ioctl(fd, EVIOCGVERSION, &version);  
    printf("evdev driver version is 0x%x: %d.%d.%d\n",  
                    version, version>>16, (version>>8) & 0xff, version & 0xff);  
  
    while (1) {  
        ret = read(fd, &ev, sizeof(struct input_event));  
        if (ret < 0) {  
            printf("read event error!\n");  
            exit(1);  
        }  
          
        if (ev.type == EV_KEY)  
            printf("type %d,code %d, value %d\n", ev.type, ev.code, ev.value);  
    }  
      
    return 0;  
} 
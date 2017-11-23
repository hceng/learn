#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#define msleep(x) usleep(x*1000)

int main(int argc, char **argv)
{
    int fd;
    int val;

    if (argc != 2)
    {
        printf("%s+val\n",argv[0]);
        return -1;
    }

    fd = open("/dev/tiny4412_backlight", O_RDWR);
     
    if (fd < 0)
    {
        printf("can't open /dev/tiny4412_backlight\n");
        return 0;
    }
    val = strtoul(argv[1], NULL, 0);   

    printf("app: val = %d",val);    

    write(fd, &val, 1);

    return 0;	
}

/*************************************************************************
    > File Name: test.c
    > Author: hceng
    > Mail: huangcheng.job@foxmail.com 
    > Created Time: Fri 21 Apr 2017 11:23:23 PM PDT
 ************************************************************************/
/*检查是否有U盘插上，没有则卸载一次/media/sda1，保证下次自动挂载U盘正常*/
/*定时器相关*/
static int count = 0;  
static struct itimerval oldtv;  
static unsigned char usb_umount_ok = 0;
void set_timer()  
{  
    struct itimerval itv;  
    itv.it_interval.tv_sec = 1;  
    itv.it_interval.tv_usec = 0;  
    itv.it_value.tv_sec = 1;  
    itv.it_value.tv_usec = 0;  
    setitimer(ITIMER_REAL, &itv, &oldtv);  
} 

/*查看/sys/bus/usb/devices/目录下是否有1-1，有则表示有U盘*/
static unsigned int judge_usb(void) 
{
    FILE *fd;  
    fd = fopen("/sys/bus/usb/devices/1-1/version","r");

    if(fd == NULL) 
        return 0;  
    else fclose(fd);
        usb_umount_ok = 0;
        return 1;
}

/*检查usb是否拔出，从而确定是否卸载*/
static void colse_usb(void)
{
    if(!judge_usb() && usb_umount_ok==0) 
    {
        system("cd /;umount /media/sda1;rm -r /media/sd*;echo umount usb ok!");
        usb_umount_ok = 1;
    }
    
}

void signal_handler(int m)  
{   
    colse_usb();    
} 

int main(void)
{
    signal(SIGALRM, signal_handler);  
    set_timer(); 
    while(1);

    return 0;
}

#include <linux/module.h>  
#include <linux/version.h>  
#include <linux/init.h>  
#include <linux/kernel.h>  
#include <linux/types.h>  
#include <linux/interrupt.h>  
#include <linux/list.h>  
#include <linux/timer.h>  
#include <linux/init.h>  
#include <linux/serial_core.h>  
#include <linux/platform_device.h>


/* 分配/设置/注册一个platform_device */
static struct resource led_resource[] = {  
    [0] = { //GPBCON  
        .start = 0x56000050,  
        .end   = 0x56000050 + 8 - 1,  
        .flags = IORESOURCE_MEM, //一般地址类的操作就用这个flags  
    },  
    [1] = { //中断号  
        .start = 5,  
        .end   = 5,  
        .flags = IORESOURCE_IRQ, //在这个例子中和中断并没有半毛钱的关系，纯粹就是表示一种序号的意思  
    }  
  
};  
  
static void led_release(struct device * dev)  
{  

}  


/* 构造platform_device结构体 */  
static struct platform_device led_dev = {  
    .name          = "myled_platform",  
    .id       	   = -1,  
    .num_resources = ARRAY_SIZE(led_resource),  
    .resource      = led_resource,  
    .dev = {   
        .release = led_release,   
    },  
};  


/* 向platform_bus注册一个设置好的platform_device */  
static int led_dev_init(void)  
{  
    platform_device_register(&led_dev);  
    return 0;  
}  
  
static void led_dev_exit(void)  
{  
    platform_device_unregister(&led_dev);  
}  
  
module_init(led_dev_init);  
module_exit(led_dev_exit);  
  
MODULE_LICENSE("GPL");  
MODULE_AUTHOR("hceng");  
MODULE_DESCRIPTION("Learn for platform");



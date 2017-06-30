#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/device.h>


static struct class *led_class;
static struct class_device	*led_dev;

volatile unsigned long *pa_cfg0;
volatile unsigned long *pa_dat;
volatile unsigned long *pa_pul0;


static int led_drv_open(struct inode *inode, struct file *file)
{
	//PA2 set output.
	printk("led_drv_open\n");

	*pa_cfg0 &= ~(0x7<<8);
	*pa_cfg0 |=  (0x1<<8);
	
	return 0;
}

static ssize_t led_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	int val;

	printk("led_drv_write\n");

	copy_from_user(&val, buf, count); 

	if (val == 1)
	{
		//PA2 set 1, for led on.
		*pa_dat |= (1<<2);
		
	}
	else
	{
		//PA2 set 0, for led on.
		*pa_dat &= ~(1<<2);
	}
	
	return 0;
}


static struct file_operations led_drv_fops = {
    .owner  =   THIS_MODULE,    
    .open   =   led_drv_open,     
	.write	=	led_drv_write,	   
};


int major;
static int led_init(void)
{
  printk("Drvice insmod ok.\n");

  major = register_chrdev(0, "led_drv", &led_drv_fops);
  
  led_class = class_create(THIS_MODULE, "led_drv");
  led_dev = device_create(led_class, NULL, MKDEV(major, 0), NULL, "led");

  //Allwinner_H3_Datasheet_V1.2.pdf P316
  pa_cfg0 = (volatile unsigned long *)ioremap(0x01c20800, 4);
  pa_dat = (volatile unsigned long *)ioremap(0x01c20800+0x10, 4);
    
  return 0;
}

static void led_exit(void)
{
  printk("Drvice exit ok.\n");

  unregister_chrdev(major, "led_drv");
  device_destroy(led_class, MKDEV(major, 0));
  class_destroy(led_class);
  iounmap(pa_cfg0);
  iounmap(pa_dat);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR ("hceng");
MODULE_DESCRIPTION ("for nano_pi neo led drvice.");



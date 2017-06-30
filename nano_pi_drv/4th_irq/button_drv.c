#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/device.h>


static struct class *button_class;
static struct class_device	*button_dev;

volatile unsigned long *pa_cfg0;
volatile unsigned long *pa_dat;


static int button_drv_open(struct inode *inode, struct file *file)
{
	//PA3 set input.
	printk("button_drv_open\n");

	*pa_cfg0 &= ~(0x7<<12);
	*pa_cfg0 |=  (0x0<<12);
	
	return 0;
}

ssize_t button_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned int key_val;
	int regval;
	
	//printk("button_drv_read\n");
	
	regval = *pa_dat;
	key_val = (regval & (1<<3)) ? 1 : 0;

	//printk("pa_dat = %x\n",*pa_dat);
	//printk("regval = %x\n",regval);
	//printk("key_val = %d\n",key_val);

	copy_to_user(buf, &key_val, 4); 

	return sizeof(key_val);
}


static struct file_operations button_drv_fops = {
    .owner  =   THIS_MODULE,    
    .open   =   button_drv_open,     
	.read	=	button_drv_read,	   
};


int major;
static int button_init(void)
{
  printk("Drvice insmod ok.\n");

  major = register_chrdev(0, "button_drv", &button_drv_fops);
  
  button_class = class_create(THIS_MODULE, "button_drv");
  button_dev = device_create(button_class, NULL, MKDEV(major, 0), NULL, "button");

  //Allwinner_H3_Datasheet_V1.2.pdf P316
  pa_cfg0 = (volatile unsigned long *)ioremap(0x01c20800, 4);
  pa_dat = (volatile unsigned long *)ioremap(0x01c20800+0x10, 4);
    
  return 0;
}

static void button_exit(void)
{
  printk("Drvice exit ok.\n");

  unregister_chrdev(major, "button_drv");
  device_destroy(button_class, MKDEV(major, 0));
  class_destroy(button_class);
  iounmap(pa_cfg0);
  iounmap(pa_dat);
}

module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR ("hceng");
MODULE_DESCRIPTION ("for nano_pi neo button drvice (no irq).");



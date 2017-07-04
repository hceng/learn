#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <linux/device.h>
#include <mach/gpio.h>
#include <linux/interrupt.h>
#include <mach/gpio.h>

#include <linux/sched.h>

#include <linux/poll.h>


static struct class *button_class;
static struct class_device	*button_dev;


//PL11
volatile unsigned long *pl_cfg1;
volatile unsigned long *pl_dat;
volatile unsigned long *pl_int1;
volatile unsigned long *pl_int_ctl;
volatile unsigned long *pl_int_sta;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

static volatile int ev_press = 0;

struct pin_desc{
	unsigned int pin;
	unsigned int key_val;
};

static unsigned int key_val;

struct pin_desc pins_desc[1] = {
	{GPIOL(11), 0x01},
};
	
static irqreturn_t buttons_irq(int irq, void *dev_id)
{
	struct pin_desc * pindesc = (struct pin_desc *)dev_id;
	unsigned int pinval = 0;
	
	int regval;

	printk("buttons_irq.\n");
	
	if(pindesc->pin == GPIOL(11)) //pinval = gpio_sw_data_get(pindesc->pin);
	{
		regval = *pl_dat;
		pinval = (regval & (1<<11)) ? 1 : 0;//get pin val.
		
		*pl_int_sta |=  (1<<11);//P350  clean irq state.
		printk("pinval = %x.\n",pinval);
	}

	if (pinval)
	{
		key_val = 0x10 | pindesc->key_val;
		printk("key_val = %x.\n",key_val);
	}
	else
	{
		key_val = pindesc->key_val;
		printk("key_val = %x.\n",key_val);
	}
	
    ev_press = 1;                  
    wake_up_interruptible(&button_waitq);  
	
	return IRQ_RETVAL(IRQ_HANDLED);
}

struct semaphore button_lock;
static int button_drv_open(struct inode *inode, struct file *file)
{ 
	//PL11 request_irq.
	printk("button_drv_open\n");


	if(file->f_flags & O_NONBLOCK)
	{
		if(down_trylock(&button_lock))
			printk("O_NONBLOCK.\n");
			return -EBUSY;
	}
	else
	{
		printk("NONBLOCK.\n");
		down(&button_lock);
	}
	
	*pl_cfg1 &= ~(0x7<<12);//P347
	*pl_cfg1 |=  (0x6<<12);

	*pl_int1 &= ~(0x7<<16);//P349
	*pl_int1 |=  (0x4<<16);//0x2: High Level;0x3: Low Level;0x4: Double Edge

	*pl_int_ctl &= ~(0x1<<11);//P350
	*pl_int_ctl |=  (0x1<<11);

	//request_irq(SUNXI_IRQ_EINTL,  buttons_irq, (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING), "button_PL11",  &pins_desc[0]);
	request_irq(SUNXI_IRQ_EINTL,  buttons_irq, IRQF_TRIGGER_NONE, "button_PL11",  &pins_desc[0]);

	return 0;
}

ssize_t button_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	printk("button_drv_read.\n");

	/*if (size != 4)
		return -EINVAL;*/
		
	if (file->f_flags & O_NONBLOCK)
	{
		if (!ev_press)
			return -EAGAIN;
	}
	else
	{
		wait_event_interruptible(button_waitq, ev_press);
	}

	copy_to_user(buf, &key_val, 4);
	ev_press = 0;
	
	return 1;
}

static unsigned button_drv_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	printk("button_drv_poll.\n");
	
	poll_wait(file, &button_waitq, wait); 
	
	if (ev_press)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}


int button_drv_close(struct inode *inode, struct file *file)
{
	printk("button_drv_close.\n");
	
	free_irq(SUNXI_IRQ_EINTL, &pins_desc[0]);

	up(&button_lock);

	return 0;
}


static struct file_operations button_drv_fops = {
    .owner  =   THIS_MODULE,    
    .open   =   button_drv_open,     
	.read	=	button_drv_read,	
	.poll    =  button_drv_poll,
	.release =  button_drv_close,	
};


int major;
static int button_init(void)
{
  printk("Drvice insmod ok.\n");

  major = register_chrdev(0, "button_drv", &button_drv_fops);
  
  button_class = class_create(THIS_MODULE, "button_drv");
  button_dev = device_create(button_class, NULL, MKDEV(major, 0), NULL, "button");

  //Allwinner_H3_Datasheet_V1.2.pdf P345  P340
  pl_cfg1 = (volatile unsigned long *)ioremap(0x01F02C00+0x04, 4);
  pl_dat  = (volatile unsigned long *)ioremap(0x01F02C00+0x10, 4);
  pl_int1 = (volatile unsigned long *)ioremap(0x01F02C00+0x200+0x04, 4);
  pl_int_ctl = (volatile unsigned long *)ioremap(0x01F02C00+0x200+0x10, 4);
  pl_int_sta = (volatile unsigned long *)ioremap(0x01F02C00+0x200+0x14, 4);

  sema_init(&button_lock, 1);
    
  return 0;
}

static void button_exit(void)
{
  printk("Drvice exit ok.\n");

  unregister_chrdev(major, "button_drv");
  device_destroy(button_class, MKDEV(major, 0));
  class_destroy(button_class);
  iounmap(pl_cfg1);
  iounmap(pl_dat);
  iounmap(pl_int1);
  iounmap(pl_int_ctl);
  iounmap(pl_int_sta);
  
}

module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR ("hceng");
MODULE_DESCRIPTION ("for nano_pi neo button drvice (irq).");


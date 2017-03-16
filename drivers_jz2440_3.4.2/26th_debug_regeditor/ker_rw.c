#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
//#include <asm/arch/regs-gpio.h>
//#include <asm/hardware.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/device.h>

#define KER_RW_R8      100
#define KER_RW_R16     101
#define KER_RW_R32     102

#define KER_RW_W8      103
#define KER_RW_W16     104
#define KER_RW_W32     105


static int major;
static struct class *class;
static struct class_device	*ker_dev;


static long ker_rw_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	volatile unsigned char  *p8;
	volatile unsigned short *p16;
	volatile unsigned int   *p32;
	unsigned int val;
	unsigned int addr;

	unsigned int buf[2];

	//printk("enter ker_rw_ioctl\n");

	if (copy_from_user(buf, (const void __user *)arg, 8))
		printk("error\n");
	addr = buf[0];
	val  = buf[1];
	//printk("ker_ioctl: 0x%x, 0x%x\n", buf[0], buf[1]);
	
	p8  = (volatile unsigned char *)ioremap(addr, 4);
	p16 = p8;
	p32 = p8;

	switch (cmd)
	{
		case KER_RW_R8:
		{
			val = *p8;
			copy_to_user((void __user *)(arg+4), &val, 4);
			break;
		}

		case KER_RW_R16:
		{
			val = *p16;
			copy_to_user((void __user *)(arg+4), &val, 4);
			break;
		}

		case KER_RW_R32:
		{
			val = *p32;
			copy_to_user((void __user *)(arg+4), &val, 4);
			break;
		}

		case KER_RW_W8:
		{
			*p8 = val;
			break;
		}

		case KER_RW_W16:
		{
			*p16 = val;
			break;
		}

		case KER_RW_W32:
		{
			*p32 = val;
			break;
		}
	}

	iounmap(p8);
	return 0;
}

static struct file_operations ker_rw_ops = {
	.owner            = THIS_MODULE,
	.unlocked_ioctl   = ker_rw_ioctl,
};

static int ker_rw_init(void)
{
	major = register_chrdev(0, "ker_rw", &ker_rw_ops);

	class = class_create(THIS_MODULE, "ker_rw");

	/* 为了让mdev根据这些信息来创建设备节点 */
	ker_dev = device_create(class, NULL, MKDEV(major, 0), NULL, "ker_rw"); /* /dev/ker_rw */
	
	return 0;
}

static void ker_rw_exit(void)
{
	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, "ker_rw");
}

module_init(ker_rw_init);
module_exit(ker_rw_exit);


MODULE_LICENSE("GPL");


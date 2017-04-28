/*设备驱动中，有些资源只能被单一进程访问，这里用sem确保单一访问*/
#include <linux/module.h>  
#include <linux/init.h>  
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/semaphore.h>//信号量头文件

static struct class *testdrv_class;
static struct class_device	*testdrv_class_dev;

struct semaphore sem;

static int test_drv_open(struct inode *inode, struct file *file)
{
	printk("kernel:down before sem.count =  %d \n", sem.count);
	down(&sem);
	/*临界资源*/
	printk("kernel:down after sem.count =  %d \n", sem.count);

	return 0;
}

static int test_drv_release(struct inode *inode, struct file *file)
{
	printk("kernel:up before sem.count =  %d \n", sem.count);
	up(&sem);
	printk("kernel:up after sem.count =  %d \n", sem.count);

	return 0;
}

static struct file_operations test_drv_fops = {
    .owner   =   THIS_MODULE,   
    .open    =   test_drv_open,     
	.release =	 test_drv_release,	   
};

int major;
static int test_drv_init(void)
{
	major = register_chrdev(0, "test_drv", &test_drv_fops);

	testdrv_class = class_create(THIS_MODULE, "testdrv");

	testdrv_class_dev = device_create(testdrv_class, NULL, MKDEV(major, 0), NULL, "semaphoretest"); 

	sema_init(&sem, 2);//允许同时2个进程访问临界资源

	printk("kernel: init ok!\n");
	
	return 0;
}

static void test_drv_exit(void)
{
	unregister_chrdev(major, "test_drv"); 

	device_destroy(testdrv_class,  MKDEV(major, 0));	
	
	class_destroy(testdrv_class);

	printk("kernel: exit ok!\n");
}

module_init(test_drv_init);
module_exit(test_drv_exit);

MODULE_LICENSE("GPL");  
MODULE_AUTHOR("hceng");  
MODULE_DESCRIPTION("Learn for semaphore");

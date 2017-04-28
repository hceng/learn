#include <linux/module.h>  
#include <linux/init.h>  

#include <asm/atomic.h>//原子变量操作的头文件
#include <asm/bitops.h>//原子位操作的头文件


atomic_t a1 = ATOMIC_INIT(1);//定义和设置原子量；
unsigned long int a2;//操作函数传入参数的数据类型是unsigned long int；

static int test_drv_init(void)
{
	
	//测试原子变量操作：
	printk("test a1\nKERN_INFO:atomic_read(): a1 =  %d\n", a1); 
	
	atomic_add(4, &a1);
	printk("KERN_INFO:atomic_add(): a1 =  %d\n", a1); 

	atomic_dec(&a1);
	printk("KERN_INFO:atomic_dec(): a1 =  %d\n", a1); 
	
	printk("KERN_INFO:atomic_dec_and_test(): judge_a1 = %d, new_a1 = %d\n",\
		atomic_dec_and_test(&a1),atomic_read(&a1));

	//测试原子位操作：
	set_bit(0,&a2);//设置a的0位为1;
	printk("test a2\nKERN_INFO:set_bit(): a2 =  %ld\n", a2); 

	printk("KERN_INFO:test_and_clear_bit(): return_a2 = %d, new_a2 = %ld\n",\
		test_and_clear_bit(0, &a2), a2);
	
	printk("KERN_INFO:test_and_set_bit(): return_a2 = %d, new_a2 = %ld\n",\
		test_and_set_bit(0, &a2), a2);

	return 0;
}

static void test_drv_exit(void)
{
	
}

module_init(test_drv_init);
module_exit(test_drv_exit);

MODULE_LICENSE("GPL");  
MODULE_AUTHOR("hceng");  
MODULE_DESCRIPTION("Learn for atomic");

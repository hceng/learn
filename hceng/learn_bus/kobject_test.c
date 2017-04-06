/*
	目的：在/sys目录下添加一个名为kobject_test的目录名，
		  并在该目录下添加一个kobject_test_attr的文件，这个文件为属性文件；
    功能：koject_test_show()函数显示属性的值；
    	  kobject_test_store()函数向属性中写入一个值；
*/
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/stat.h>

/*声明释放kobject结构体函数*/
void kobject_test_release(struct kobject *kobject);

/*读属性的函数*/
ssize_t kobject_test_show(struct kobject *kobject, struct attribute *attr, char *buf);

/*写属性的函数*/
ssize_t kobject_test_store(struct kobject *kobject, struct attribute *attr, const char *buf, size_t count);

/*定义了一个名为kobject_test，可以读写的属性*/
struct attribute test_attr = {
	.name = "kobject_test_attr",
	.mode = S_IRWXUGO,
};

/*该object只有一个属性*/
static struct attribute *def_attrs[] = {
	&test_attr,
	NULL,
};

/*操作函数*/
struct sysfs_ops obj_test_sysops = {
	.show = kobject_test_show,
	.store = kobject_test_store,
};

/*kobject属性*/
struct kobj_type ktype = {
	.release = kobject_test_release,
	.sysfs_ops = &obj_test_sysops,
	.default_attrs = def_attrs,
};
	
/*释放kobject结构体函数*/
void kobject_test_release(struct kobject *kobject)
{
	printk("kobject_test: kobject_test_release(). \n");
}

/*该函数用于读取一个属性的名字*/
ssize_t kobject_test_show(struct kobject *kobject, struct attribute *attr, char *buf)
{
	printk("call kobject_test_show(). \n");
	printk("attrname:%s.\n", attr->name);
	sprintf(buf, "%s\n", attr->name);
	return strlen(attr->name)+2;
}

/*该函数用于写入一个属性的值*/
ssize_t kobject_test_store(struct kobject *kobject, struct attribute *attr, const char *buf, size_t count)
{
	printk("call kobject_test_store(). \n");
	printk("write: %s\n", buf);
	strcpy(attr->name, buf);
	return count;
}

/*入口函数*/
struct kobject kobj;
static int kobject_test_init()
{
	printk("kobject test_init().\n");
	kobject_init_and_add(&kobj, &ktype, NULL, "kobject_test");
	return 0;
}

/*出口函数*/
static int kobject_test_exit()
{
	printk("kobject test exit.\n");
	kobject_del(&kobj);
	return 0;
}

module_init(kobject_test_init);
module_exit(kobject_test_exit);
MODULE_AUTHOR("hceng");
MODULE_LICENSE("Dual BSD/GPL");

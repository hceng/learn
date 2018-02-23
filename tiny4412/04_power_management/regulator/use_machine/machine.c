#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/mfd/core.h>
#include <linux/regulator/machine.h>


#if 1

static struct regulator_consumer_supply tiny4412_regulator_supplies[] = {
	REGULATOR_SUPPLY("VCC_LCD", "11c00000.lcd_s702"),//consumer的电源引脚名称;consumer的名字
};
    
#else

static struct regulator_consumer_supply tiny4412_regulator_supplies[] = {
	REGULATOR_SUPPLY("VCC_LCD", "lcd_s702"),
};
    
#endif

static struct regulator_init_data tiny4412_regulator_init_data = {
	.constraints = {
        //.name = "tiny4412_regulator",
		.min_uV			= 1000000,
		.max_uV			= 1000000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL,
		.valid_ops_mask		= REGULATOR_CHANGE_STATUS,
        .boot_on = 0,
        .always_on = 0,
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= tiny4412_regulator_supplies,
};

static void tiny4412_regulator_release(struct device * dev)
{
}

static struct platform_device tiny4412_regulator_dev = {
    .name         = "tiny4412_regulator",
    .id       = -1,
    .dev = { 
    	.release       = tiny4412_regulator_release, 
		.platform_data = &tiny4412_regulator_init_data,
	},
};

static int tiny4412_regulator_machine_init(void)
{
    printk("enter %s\n", __func__);

    platform_device_register(&tiny4412_regulator_dev);
	return 0;
}

static void tiny4412_regulator_machine_exit(void)
{
    printk("enter %s\n", __func__);

    platform_device_unregister(&tiny4412_regulator_dev);
}

module_init(tiny4412_regulator_machine_init);
module_exit(tiny4412_regulator_machine_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hceng <huangcheng.job@foxmail.com>");
MODULE_DESCRIPTION("Tiny4412 machine driver.");
MODULE_ALIAS("Exynos4412_machine");
MODULE_VERSION("V1.0");

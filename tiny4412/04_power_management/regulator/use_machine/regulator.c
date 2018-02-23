#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/mfd/core.h>
#include <linux/io.h>


static int regulator_states = 0;
static int tiny4412_regulator_enable(struct regulator_dev *rdev)
{
    printk("enter %s\n", __func__);

    printk("------LCD Power Open.------\n");
    regulator_states = 1;

    return 0;
}

static int tiny4412_regulator_disable(struct regulator_dev *rdev)
{
    printk("enter %s\n", __func__);

    printk("------LCD Power Close.------\n");
    regulator_states = 0;

    return 0;
}

static int tiny4412_regulator_is_enabled(struct regulator_dev *rdev)
{
    printk("enter %s\n", __func__);

    printk("------LCD Power Test.------\n");
    if (regulator_states)
        return 1;
    else
        return 0;
}

static struct regulator_ops tiny4412_regulator_ops =
{
    .enable		= tiny4412_regulator_enable,
    .disable	= tiny4412_regulator_disable,
    .is_enabled	= tiny4412_regulator_is_enabled,
};

static struct regulator_desc tiny4412_regulator_desc =
{
    .name		= "tiny4412_regulator",
    .ops		= &tiny4412_regulator_ops,
    .type		= REGULATOR_VOLTAGE,//电压源
    .id		    = 0,
    .owner		= THIS_MODULE,
    .n_voltages	= 1,//能提供的电压数量
};

static struct regulator_dev *tiny4412_regulator_dev;
static int tiny4412_regulator_probe(struct platform_device *pdev)
{
    struct regulator_config config = { };
    config.dev = &pdev->dev;
    config.init_data = dev_get_platdata(&pdev->dev);

    printk("enter %s\n", __func__);

    tiny4412_regulator_dev = devm_regulator_register(&pdev->dev, &tiny4412_regulator_desc, &config);
    if (IS_ERR(tiny4412_regulator_dev))
    {
        printk("devm_regulator_register error!\n");
        return PTR_ERR(tiny4412_regulator_dev);
    }

    return 0;
}

static int tiny4412_regulator_remove(struct platform_device *pdev)
{
    printk("enter %s\n", __func__);

    devm_regulator_unregister(&pdev->dev, tiny4412_regulator_dev);

    return 0;
}


struct platform_driver tiny4412_regulator_drv =
{
    .probe		= tiny4412_regulator_probe,
    .remove		= tiny4412_regulator_remove,
    .driver		= {
        .name	= "tiny4412_regulator",
    }
};

static int tiny4412_regulator_init(void)
{
    printk("enter %s\n", __func__);

    platform_driver_register(&tiny4412_regulator_drv);
    return 0;
}

static void tiny4412_regulator_exit(void)
{
    printk("enter %s\n", __func__);

    platform_driver_unregister(&tiny4412_regulator_drv);
}

module_init(tiny4412_regulator_init);
module_exit(tiny4412_regulator_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hceng <huangcheng.job@foxmail.com>");
MODULE_DESCRIPTION("Tiny4412 regulator driver.");
MODULE_ALIAS("Exynos4412_regulator");
MODULE_VERSION("V1.0");

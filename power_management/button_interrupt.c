#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>

typedef struct 
{
    int gpio;
    int irq;
    char name[20];
}button_data_t;

static button_data_t *data = NULL;

static irqreturn_t button_isr(int irq, void *dev_id)
{
    button_data_t *data = dev_id;

    printk("enter %s\n", __func__);
    printk("%s: gpio:%d, irq: %d\n", data->name, data->gpio, data->irq);

    return IRQ_HANDLED;
}

static int button_probe(struct platform_device *pdev) 
{
    struct device *dev = &pdev->dev;
    int irq_gpio = -1;
    int irq = -1;
    int ret = 0;
    int i = 0;

    printk("enter %s\n", __func__);

    if (!dev->of_node) {
        dev_err(dev, "no platform data.\n");
        goto err1;
    }

    data = devm_kmalloc(dev, sizeof(*data)*4, GFP_KERNEL);
    if (!data) {
        dev_err(dev, "no memory.\n");
        goto err0;
    }

    for (i = 3; i >= 0; i--) 
    {
        sprintf(data[i].name, "tiny4412,gpx3_%d", i+2);

        irq_gpio = of_get_named_gpio(dev->of_node, data[i].name, 0);//通过名字获取gpio
        if (irq_gpio < 0) 
        {
            dev_err(dev, "Looking up %s property in node %s failed %d\n", data[i].name, dev->of_node->full_name, irq_gpio);
            goto err1;
        }

        data[i].gpio = irq_gpio;

        irq = gpio_to_irq(irq_gpio);    //将gpio转换成对应的中断号
        if (irq < 0) 
        {
            dev_err(dev, "Unable to get irq number for GPIO %d, error %d\n", irq_gpio, irq);
            goto err1;
        }

        data[i].irq = irq;

        printk("%s: gpio: %d ---> irq (%d)\n", __func__, irq_gpio, irq);
        
        //注册中断
        ret = devm_request_any_context_irq(dev, irq, button_isr, IRQF_TRIGGER_FALLING, data[i].name, data+i);
        if (ret < 0) 
        {
            dev_err(dev, "Unable to claim irq %d; error %d\n",irq, ret);
            goto err1;
        }
        
        //设置为中断唤醒源
        irq_set_irq_wake(irq, 1);
    }

    return 0;

err1:
    devm_kfree(dev, data);
err0:
    return -EINVAL;
}

static int button_remove(struct platform_device *pdev) {

    printk("enter %s\n", __func__);

    return 0;
}

static const struct of_device_id int_button_dt_ids[] = {
    { .compatible = "tiny4412,button_interrupt", },
    {},
};

MODULE_DEVICE_TABLE(of, int_button_dt_ids);

static struct platform_driver button_driver = {
    .driver        = {
        .name      = "button_interrupt",
        .of_match_table    = of_match_ptr(int_button_dt_ids),
    },
    .probe         = button_probe,
    .remove        = button_remove,
};

static int button_init(void)
{
    int ret;

    printk("enter %s\n", __func__);
    
    ret = platform_driver_register(&button_driver);
    if (ret)
        printk(KERN_ERR "button_init: failed: %d\n", ret);

    return ret;
}


static void button_exit(void)
{
    int i;
    
    printk("enter %s\n", __func__);
    
    for (i = 3; i >= 0; i--) 
    {
        irq_set_irq_wake(data[i].irq, 0);
    }
    
    platform_driver_unregister(&button_driver);
}

module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hceng <huangcheng.job@foxmail.com>");
MODULE_DESCRIPTION("Tiny4412 button interrupt drvice.");
MODULE_ALIAS("button");
MODULE_VERSION("V1.0");
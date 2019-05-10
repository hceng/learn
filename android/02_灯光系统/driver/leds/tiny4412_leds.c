
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include <linux/gpio.h>
#include <linux/leds.h>
#include <plat/gpio-cfg.h>

struct led_desc {
    int gpio;
    char *name;
};

static struct led_desc led_gpios[] = {
    {EXYNOS4212_GPM4(0), "led1"},
    {EXYNOS4212_GPM4(1), "led2"},
    {EXYNOS4212_GPM4(2), "led3"},
    {EXYNOS4212_GPM4(3), "led4"},
};
static int pin_num = sizeof(led_gpios)/sizeof(led_gpios[0]);

struct tiny4412_led_data {
    struct led_classdev led_dev;
    int gpio;
    struct mutex lock; 
};

static struct tiny4412_led_data *led_devs;

void tiny4412_brightness_set(struct led_classdev *led_cdev,
                      enum led_brightness brightness)
{
    struct tiny4412_led_data *dev = (struct tiny4412_led_data *)led_cdev;
    
    printk(KERN_DEBUG "Passed %s %d \n",__FUNCTION__,__LINE__);

    led_cdev->brightness = brightness;

    if (brightness != LED_OFF)
        gpio_set_value(dev->gpio, 0);
    else
        gpio_set_value(dev->gpio, 1);
}


static int leds_drv_init(void)  
{   
    int i, ret;
    
    printk(KERN_DEBUG "Passed %s %d \n",__FUNCTION__,__LINE__);
    
    //1. alloc led_classdev
    led_devs = kzalloc(sizeof(struct tiny4412_led_data) * pin_num, GFP_KERNEL);
    if (led_devs == NULL) {
        printk(KERN_ERR "No memory for device\n");
        return -ENOMEM;
    }
    
    for (i = 0; i < pin_num; i++)
    {
        mutex_init(&led_devs[i].lock);
        mutex_lock(&led_devs[i].lock);

        //2. set led_classdev
        led_devs[i].led_dev.max_brightness = LED_FULL; //255
        led_devs[i].led_dev.brightness_set = tiny4412_brightness_set;
        led_devs[i].led_dev.flags = LED_CORE_SUSPENDRESUME;
        led_devs[i].led_dev.brightness = LED_OFF;
        led_devs[i].led_dev.name = led_gpios[i].name;
        led_devs[i].led_dev.default_trigger = "none";
        
        led_devs[i].gpio = led_gpios[i].gpio;
        
        //3. Hardware setup(Default led off)
        s3c_gpio_cfgpin(led_gpios[i].gpio, S3C_GPIO_OUTPUT);
        if (led_devs[i].led_dev.brightness == LED_OFF)
            gpio_set_value(led_gpios[i].gpio, 1);
        else
            gpio_set_value(led_gpios[i].gpio, 0);

        //4. led_classdev_register 
        ret = led_classdev_register(NULL, &led_devs[i].led_dev);
        if (ret) {
            i--;
            while (i >= 0) {
                led_classdev_unregister(&led_devs[i].led_dev);
                i--;
            }
            mutex_unlock(&led_devs[i].lock);
            kfree(led_devs);
            return -EIO;
        }
        mutex_unlock(&led_devs[i].lock);
    }
    return 0;  
}  
  
static void leds_drv_exit(void)  
{  
    int i;
    
    printk(KERN_DEBUG "Passed %s %d \n",__FUNCTION__,__LINE__);
        
    for (i = 0; i < pin_num; i++)
    {
        mutex_lock(&led_devs[i].lock);
        
        led_classdev_unregister(&led_devs[i].led_dev);
    
        mutex_unlock(&led_devs[i].lock);
    }
    kfree(led_devs);
} 


module_init(leds_drv_init);
module_exit(leds_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hceng <huangcheng.job@foxmail.com>");
MODULE_DESCRIPTION("Tiny4412 leds driver.");
MODULE_VERSION("v1.0");
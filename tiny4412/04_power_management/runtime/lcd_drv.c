#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#include <linux/fb.h>
#include <asm/types.h>
#include <linux/suspend.h>
#include <linux/pm_runtime.h>


#define         VIDCON0                 0x00
#define         VIDCON1                 0x04
#define         VIDTCON0                0x10
#define         VIDTCON1                0x14
#define         VIDTCON2                0x18
#define         WINCON0                 0x20
#define         VIDOSD0C                0x48
#define         SHADOWCON               0x34
#define         WINCHMAP2               0x3c
#define         VIDOSD0A                0x40
#define         VIDOSD0B                0x44
#define         VIDW00ADD0B0            0xA0
#define         VIDW00ADD1B0            0xD0
#define         CLK_SRC_LCD0            0x234
#define         CLK_SRC_MASK_LCD        0x334
#define         CLK_DIV_LCD             0x534
#define         CLK_GATE_IP_LCD         0x934
#define         LCDBLK_CFG              0x00
#define         LCDBLK_CFG2             0x04
#define         LCD_LENTH               800
#define         LCD_WIDTH               480
#define         BITS_PER_PIXEL          32

static struct fb_info *tiny4412_lcd;
static volatile void __iomem *lcd_regs_base;
static volatile void __iomem *lcdblk_regs_base;
static volatile void __iomem *lcd0_configuration;//Configures power mode of LCD0.0x10020000+0x3C80
static volatile void __iomem *clk_regs_base;


static u32 pseudo_palette[16];
static struct resource  *res0, *res1, *res2, *res3;

/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
    chan &= 0xFFFF;//保留低16位
    chan >>= 16 - bf->length;//保留高bf->length位
    return chan << bf->offset;//返回保留的位，且在原位置
}
static int cfb_setcolreg(unsigned int regno, unsigned int red,
                         unsigned int green, unsigned int blue,
                         unsigned int transp, struct fb_info *info)
{

    unsigned int color = 0;
    uint32_t *p;
    color  = chan_to_field(red,   &info->var.red);
    color |= chan_to_field(green, &info->var.green);
    color |= chan_to_field(blue,  &info->var.blue);

    p = info->pseudo_palette;
    p[regno] = color;
    return 0;
}

static int s702_lcd_open(struct fb_info *info, int user)
{
    struct device *dev = info->dev;
    int ret;

    printk("enter %s\n", __func__);

    ret = pm_runtime_get_sync(dev);
    if (ret < 0 && ret != -EACCES)
    {
        //pm_runtime_put_sync(dev);
        pm_runtime_mark_last_busy(dev);
        pm_runtime_put_autosuspend(dev);
        return ret;
    }

    return 0;
}
static int s702_lcd_release(struct fb_info *info, int user)
{
    struct device *dev = info->dev;

    printk("enter %s\n", __func__);

    //pm_runtime_put_sync(dev);
    pm_runtime_mark_last_busy(dev);
    pm_runtime_put_sync_autosuspend(dev);

    return 0;
}

static struct fb_ops tiny4412_lcdfb_ops =
{
    .owner              = THIS_MODULE,
    .fb_setcolreg       = cfb_setcolreg, //设置调色板，实现伪颜色表
    .fb_fillrect        = cfb_fillrect,  //填充矩形
    .fb_copyarea        = cfb_copyarea,  //数据复制
    .fb_imageblit       = cfb_imageblit, //图形填充

    .fb_open            = s702_lcd_open,
    .fb_release         = s702_lcd_release
};

static int lcd_suspend_notifier(struct notifier_block *nb, unsigned long event, void *dummy)
{
    switch (event)
    {
    case PM_SUSPEND_PREPARE:
        printk("lcd suspend notifiler test: PM_SUSPEND_PREPARE\n");
        return NOTIFY_OK;
    case PM_POST_SUSPEND:
        printk("lcd suspend notifiler test: PM_POST_SUSPEND\n");
        return NOTIFY_OK;

    default:
        return NOTIFY_DONE;
    }
}

static struct notifier_block lcd_pm_notif_block =
{
    .notifier_call = lcd_suspend_notifier,
};

static int lcd_probe(struct platform_device *pdev)
{
    int ret;
    unsigned int temp;

    printk("enter %s\n", __func__);

    /* 1. 分配一个fb_info */
    tiny4412_lcd = framebuffer_alloc(0, NULL);                        //不要额外空间设置私有数据
    if(!tiny4412_lcd)
    {
        return  -ENOMEM;
    }

    /* 2. 设置 */
    /* 2.1 设置 fix 固定的参数 */
    strcpy(tiny4412_lcd->fix.id, "s702");                              //设置fix名称
    tiny4412_lcd->fix.smem_len = LCD_LENTH * LCD_WIDTH * BITS_PER_PIXEL / 8; //显存的长度=分辨率*每象素字节数
    tiny4412_lcd->fix.type     = FB_TYPE_PACKED_PIXELS;                //类型:填充式像素(常用在TFT屏幕)
    tiny4412_lcd->fix.visual   = FB_VISUAL_TRUECOLOR;                  //TFT 真彩色
    tiny4412_lcd->fix.line_length = LCD_LENTH * BITS_PER_PIXEL / 8;    //每行的长度，以字节为单位
    /* 2.2 设置 var 可变的参数 */
    tiny4412_lcd->var.xres           = LCD_LENTH;                      //x方向分辨率
    tiny4412_lcd->var.yres           = LCD_WIDTH;                      //y方向分辨率
    tiny4412_lcd->var.xres_virtual   = LCD_LENTH;                      //x方向虚拟分辨率
    tiny4412_lcd->var.yres_virtual   = LCD_WIDTH;                      //y方向虚拟分辨率
    tiny4412_lcd->var.xoffset        = 0;                              //x方向真实值和虚拟值得差值
    tiny4412_lcd->var.yoffset        = 0;                              //y方向真实值和虚拟值得差值
    tiny4412_lcd->var.bits_per_pixel = BITS_PER_PIXEL;                 //每个像素占多少位
    /* RGB:888 */
    tiny4412_lcd->var.red.length     = 8;
    tiny4412_lcd->var.red.offset     = 16;   //红
    tiny4412_lcd->var.green.length   = 8;
    tiny4412_lcd->var.green.offset   = 8;    //绿
    tiny4412_lcd->var.blue.length    = 8;
    tiny4412_lcd->var.blue.offset    = 0;    //蓝
    tiny4412_lcd->var.activate       = FB_ACTIVATE_NOW;      //使设置的值立即生效
    /* 2.3 设置操作函数 */
    tiny4412_lcd->fbops              = &tiny4412_lcdfb_ops;  //绑定操作函数
    /* 2.4 其他的设置 */
    tiny4412_lcd->pseudo_palette     = pseudo_palette;       //存放调色板所调颜色的数组
    tiny4412_lcd->screen_size        = LCD_LENTH * LCD_WIDTH * BITS_PER_PIXEL / 8;   //显存大小

    /* 3. 硬件相关的操作 */
    /* 3.1 配置GPIO用于LCD */
    //在设备树中，将 GPF0_0-GPF0_7、GPF1_0-GPF1_7、GPF2_0-GPF2_7、GPF3_0-GPF3_3
    //配置为了复用第二功能(LCD)，禁止内部上拉，驱动强度配置设置为0;
    /* 3.2 根据LCD手册设置LCD控制器, 比如VCLK的频率等 */
    //寄存器映射
    res0 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (res0 == NULL)
    {
        printk("platform_get_resource error.\n");
        return -EINVAL;
    }
    lcd_regs_base = devm_ioremap_resource(&pdev->dev, res0);
    if (lcd_regs_base == NULL)
    {
        printk("devm_ioremap_resource error.\n");
        return -EINVAL;
    }

    res1 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    if (res1 == NULL)
    {
        printk("platform_get_resource error.\n");
        return -EINVAL;
    }
    lcdblk_regs_base = devm_ioremap_resource(&pdev->dev, res1);
    if (lcdblk_regs_base == NULL)
    {
        printk("devm_ioremap_resource error.\n");
        return -EINVAL;
    }

    res2 = platform_get_resource(pdev, IORESOURCE_MEM, 2);
    if (res2 == NULL)
    {
        printk("platform_get_resource error.\n");
        return -EINVAL;
    }
    //bug：
    /*
    devm_ioremap()和devm_ioremap_resource()区别：
    devm_ioremap()可以重复map相同的地址空间，devm_ioremap_resource()不可以。
    一般SoC的中，各个硬件模块各自的memory region都有严格的划分(比如说USB host的地址空间绝对不会和flash host冲突)，
    所以一般的driver使用devm_ioremap()和devm_ioremap_resource()都行。
    但这里，应该系统已经映射过一次了，所以使用devm_ioremap_resource()会报错。
    */
    //lcd0_configuration = devm_ioremap_resource(&pdev->dev, res2);
    lcd0_configuration = devm_ioremap(&pdev->dev, res2->start, resource_size(res2));
    if (lcd0_configuration == NULL)
    {
        printk("devm_ioremap_resource error.\n");
        return -EINVAL;
    }
    *(unsigned long *)lcd0_configuration = 7; //Reset Value = 0x00000007

    res3 = platform_get_resource(pdev, IORESOURCE_MEM, 3);
    if (res3 == NULL)
    {
        printk("platform_get_resource error.\n");
        return -EINVAL;
    }
    //clk_regs_base = devm_ioremap_resource(&pdev->dev, res3);
    clk_regs_base = devm_ioremap(&pdev->dev, res3->start, resource_size(res3));
    if (clk_regs_base == NULL)
    {
        printk("devm_ioremap_resource error.\n");
        return -EINVAL;
    }
    //时钟源选择\使能时钟
    //Selects clock source for LCD_BLK
    //FIMD0_SEL:bit[3:0]=0110=SCLKMPLL_USER_T=800M
    temp = readl(clk_regs_base + CLK_SRC_LCD0);
    temp &= ~(0x0F << 0);
    temp |= (0x3 << 1);
    writel(temp, clk_regs_base + CLK_SRC_LCD0);

    //Clock source mask for LCD_BLK
    //FIMD0_MASK:Mask output clock of MUXFIMD0 (1=Unmask)
    temp = readl(clk_regs_base + CLK_SRC_MASK_LCD);
    temp |= (0x01 << 0);
    writel(temp, clk_regs_base + CLK_SRC_MASK_LCD);

    //Clock source mask for LCD_BLK
    //SCLK_FIMD0 = MOUTFIMD0/(FIMD0_RATIO + 1),分频比 1/1
    temp = readl(clk_regs_base + CLK_DIV_LCD);
    temp &= ~(0x0F << 0);
    writel(temp, clk_regs_base + CLK_DIV_LCD);

    //Controls IP clock gating for LCD_BLK
    //CLK_FIMD0:Gating all clocks for FIMD0 (1=Pass)
    temp = readl(clk_regs_base + CLK_GATE_IP_LCD);
    temp |= (0x01 << 0);
    writel(temp, clk_regs_base + CLK_GATE_IP_LCD);

    //FIMDBYPASS_LBLK0:FIMD of LBLK0 Bypass Selection (1=FIMD Bypass)
    temp = readl(lcdblk_regs_base + LCDBLK_CFG);
    temp |= (0x01 << 1);
    writel(temp, lcdblk_regs_base + LCDBLK_CFG);

    //MIE0_DISPON:MIE0_DISPON: PWM output control (1=PWM outpupt enable)
    temp = readl(lcdblk_regs_base + LCDBLK_CFG2);
    temp |= (0x01 << 0);
    writel(temp, lcdblk_regs_base + LCDBLK_CFG2);

    mdelay(1000);

    //LCD时钟:  VCLK=FIMD*SCLK/(CLKVAL+1), where CLKVAL>=1
    //800/(19+1) == 40M<80M
    temp = readl(lcd_regs_base + VIDCON0);
    temp |= (19 << 6);
    writel(temp, lcd_regs_base + VIDCON0);

    /*
     * VIDTCON1:
     * [5]:IVSYNC  ===> 1 : Inverted(反转)
     * [6]:IHSYNC  ===> 1 : Inverted(反转)
     * [7]:IVCLK   ===> 1 : Fetches video data at VCLK rising edge (上降沿触发)
     * [10:9]:FIXVCLK  ====> 01 : VCLK running
     */
    temp = readl(lcd_regs_base + VIDCON1);
    temp |= (1 << 9) | (1 << 7) | (1 << 5) | (1 << 6);
    writel(temp, lcd_regs_base + VIDCON1);

    /*
     * VIDTCON0:
     * [23:16]:  VBPD+1=tvb-tvpw=23-11=12 --> VBPD=11
     * [15:8] :  VFPD+1=tvfp=22 --> VFPD=21
     * [7:0]  :  VSPW+1=tvpw=1~20(暂取11) --> VSPW=10
     */
    temp = readl(lcd_regs_base + VIDTCON0);
    temp |= (11 << 16) | (21 << 8) | (10 << 0);
    writel(temp, lcd_regs_base + VIDTCON0);

    /*
     * VIDTCON1:
     * [23:16]:  HBPD+1=thb-hpw=46-21=25 --> HBPD=24
     * [15:8] :  HFPD+1=thfp=210 --> HFPD=209
     * [7:0]  :  HSPW+1=hpw=1~40(暂取21) --> HSPW=20
     */
    temp = readl(lcd_regs_base + VIDTCON1);
    temp |= (24 << 16) | (209 << 8)  | (20 << 0);
    writel(temp, lcd_regs_base + VIDTCON1);

    /*
     * HOZVAL = (Horizontal display size) - 1 and LINEVAL = (Vertical display size) - 1.
     * Horizontal(水平) display size : 800
     * Vertical(垂直) display size : 480
     */
    temp = ((LCD_WIDTH - 1) << 11) | (LCD_LENTH << 0);
    writel(temp, lcd_regs_base + VIDTCON2);

    /*
     * WINCON0:
     * [15]:Specifies Word swap control bit.  1 = Enables swap 低位像素存放在低字节
     * [5:2]: Selects Bits Per Pixel (BPP) mode for Window image : 1101 ===> Unpacked 25 BPP (non-palletized A:1-R:8-G:8-B:8)
     * [0]:Enables/disables video output   1 = Enables
     */
    temp = readl(lcd_regs_base + WINCON0);
    temp &= ~(0x0F << 2);
    temp |= (0X01 << 15) | (0x0D << 2) | (0x01 << 0);
    writel(temp, lcd_regs_base + WINCON0);

    //Enables Channel 0.
    temp = readl(lcd_regs_base + SHADOWCON);
    writel(temp | 0x01, lcd_regs_base + SHADOWCON);
    //Selects Channel 0
    temp = readl(lcd_regs_base + WINCHMAP2);
    temp &= ~(7 << 16);
    temp |= (0x01 << 16);//CH0FISEL:Selects Channel 0's channel.001 = Window 0
    temp &= ~(7 << 0);
    temp |= (0x01 << 0);//W0FISEL:Selects Window 0's channel.001 = Channel 0
    writel(temp, lcd_regs_base + WINCHMAP2);

    //设置OSD显示大小
    //Window Size For example. Height *  Width (number of word)
    temp = (LCD_LENTH * LCD_WIDTH) >> 1;
    writel(temp, lcd_regs_base + VIDOSD0C);
    /*
     * bit0-10 : 指定OSD图像左上像素的垂直屏幕坐标
     * bit11-21: 指定OSD图像左上像素的水平屏幕坐标
     */
    writel(0, lcd_regs_base + VIDOSD0A);
    /*
     * bit0-10 : 指定OSD图像右下像素的垂直屏幕坐标
     * bit11-21: 指定OSD图像右下像素的水平屏幕坐标
     */
    writel(((LCD_LENTH - 1) << 11) | (LCD_WIDTH - 1), lcd_regs_base + VIDOSD0B);

    //Display On: ENVID and ENVID_F are set to "1".
    temp = readl(lcd_regs_base + VIDCON0);
    writel(temp | (0x01 << 1) | (0x01 << 0), lcd_regs_base + VIDCON0);

    /* 3.3 分配显存(framebuffer), 并把地址告诉LCD控制器 */
    // tiny4412_lcd->screen_base         显存虚拟地址
    // tiny4412_lcd->fix.smem_len        显存大小，前面计算的
    // tiny4412_lcd->fix.smem_start      显存物理地址
    tiny4412_lcd->screen_base = dma_alloc_writecombine(NULL, tiny4412_lcd->fix.smem_len, (dma_addr_t *)&tiny4412_lcd->fix.smem_start, GFP_KERNEL);
    //显存起始地址
    writel(tiny4412_lcd->fix.smem_start, lcd_regs_base + VIDW00ADD0B0);
    //显存结束地址
    writel(tiny4412_lcd->fix.smem_start + tiny4412_lcd->fix.smem_len, lcd_regs_base + VIDW00ADD1B0);

    /* 4. 注册 */
    ret = register_framebuffer(tiny4412_lcd);

    /* Power Management */
    //Suspend
    ret = register_pm_notifier(&lcd_pm_notif_block);
    if(ret)
    {
        printk("failed to register pm notifier.\n");
        return  -EINVAL;
    }
    //Runtime
    pm_runtime_set_active(&pdev->dev);
    pm_runtime_use_autosuspend(&pdev->dev);//add autosleep
    pm_runtime_set_autosuspend_delay(&pdev->dev, 5000);
    pm_runtime_enable(&pdev->dev);

    return ret;
}

static int lcd_remove(struct platform_device *pdev)
{
    //Direct Off: ENVID and ENVID_F are set to "0" simultaneously.
    unsigned int temp;
    temp = readl(lcd_regs_base + VIDCON0);
    temp &= ~(0x01 << 1 | 0x01 << 0);
    writel(temp, lcd_regs_base + VIDCON0);

    pm_runtime_disable(&pdev->dev);

    unregister_framebuffer(tiny4412_lcd);
    dma_free_writecombine(NULL, tiny4412_lcd->fix.smem_len, tiny4412_lcd->screen_base, tiny4412_lcd->fix.smem_start);
    framebuffer_release(tiny4412_lcd);

    return 0;
}

static int s702_lcd_suspend(struct device *dev)
{
    //lcd休眠操作
    //Direct Off: ENVID and ENVID_F are set to "0" simultaneously.
    unsigned int temp;

    printk("enter %s\n", __func__);

    temp = readl(lcd_regs_base + VIDCON0);
    temp &= ~(0x01 << 1 | 0x01 << 0);
    writel(temp, lcd_regs_base + VIDCON0);

    return 0;
}

static int s702_lcd_resume(struct device *dev)
{
    //lcd唤醒操作
    //Display On: ENVID and ENVID_F are set to "1".
    unsigned int temp;

    printk("enter %s\n", __func__);

    temp = readl(lcd_regs_base + VIDCON0);
    writel(temp | (0x01 << 1) | (0x01 << 0), lcd_regs_base + VIDCON0);

    return 0;
}

static struct dev_pm_ops lcd_pm =
{
    .suspend = s702_lcd_suspend,
    .resume  = s702_lcd_resume,
    .runtime_suspend = s702_lcd_suspend,
    .runtime_resume  = s702_lcd_resume,
};

static const struct of_device_id lcd_dt_ids[] =
{
    { .compatible = "tiny4412, lcd_s702", },
    {},
};
MODULE_DEVICE_TABLE(of, lcd_dt_ids);

static struct platform_driver lcd_driver =
{
    .driver        = {
        .name           = "lcd_s702",
        .pm             = &lcd_pm,
        .of_match_table = of_match_ptr(lcd_dt_ids),
    },
    .probe         = lcd_probe,
    .remove        = lcd_remove,
};
static int lcd_init(void)
{
    int ret;
    printk("enter %s\n", __func__);

    ret = platform_driver_register(&lcd_driver);
    if (ret)
    {
        printk(KERN_ERR "lcd: probe fail: %d\n", ret);
    }

    return ret;
}
static void lcd_exit(void)
{
    printk("enter %s\n", __func__);

    platform_driver_unregister(&lcd_driver);
}
module_init(lcd_init);
module_exit(lcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hceng <huangcheng.job@foxmail.com>");
MODULE_DESCRIPTION("Tiny4412 LCD drvice.");
MODULE_ALIAS("Exynos4412_s702");
MODULE_VERSION("V1.0");

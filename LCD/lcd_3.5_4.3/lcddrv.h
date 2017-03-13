/*
 * FILE: lcddrv.h
 * 操作LCD控制器、调色板等的底层函数接口
 */

#ifndef __LCDDRV_H__
#define __LCDDRV_H__

#include <types.h>

#define LOWER21BITS(n)  ((n) & 0x1fffff)

#define BPPMODE_1BPP    0x8
#define BPPMODE_2BPP    0x9
#define BPPMODE_4BPP    0xA
#define BPPMODE_8BPP    0xB
#define BPPMODE_16BPP   0xC
#define BPPMODE_24BPP   0xD

#define LCDTYPE_TFT     0x3

#define ENVID_DISABLE   0
#define ENVID_ENABLE    1

#define FORMAT8BPP_5551 0
#define FORMAT8BPP_565  1

#define HSYNC_NORM      0
#define HSYNC_INV       1

#define VSYNC_NORM      0
#define VSYNC_INV       1

#define VDEN_NORM		0
#define VDEN_INV		1

#define BSWP            1
#define HWSWP           1

//TFT LCD Panel(240*320)
#define MODE_TFT_1BIT_240320     (0x4101)
#define MODE_TFT_8BIT_240320 	 (0x4102)
#define MODE_TFT_16BIT_240320    (0x4104)
#define MODE_TFT_24BIT_240320    (0x4108)

#define MODE_TFT_8BIT_480272 	 (0x410c)
#define MODE_TFT_16BIT_480272    (0x4110)

//TFT 240320
#define LCD_XSIZE_TFT_240320 	(240)	
#define LCD_YSIZE_TFT_240320 	(320)

//TFT 240320
#define HOZVAL_TFT_240320	(LCD_XSIZE_TFT_240320-1)
#define LINEVAL_TFT_240320	(LCD_YSIZE_TFT_240320-1)

#define CLKVAL_TFT_240320	(4)
// 60hz @133Mhz
// (9) 60hz @100Mhz

//Timing parameter for LTS350Q1(SAMSUNG) 
#define VBPD_240320		((6-1)&0xff)
#define VFPD_240320		((4-1)&0xff)
#define VSPW_240320		((2-1) &0x3f)
#define HBPD_240320		((11-1)&0x7f)
#define HFPD_240320		((2-1)&0xff)
#define HSPW_240320		((1-1)&0xff)



//TFT LCD Panel(640*480)
#define MODE_TFT_1BIT_640480     (0x4201)
#define MODE_TFT_8BIT_640480     (0x4202)
#define MODE_TFT_16BIT_640480    (0x4204)
#define MODE_TFT_24BIT_640480    (0x4208)

//TFT 640480
#define LCD_XSIZE_TFT_640480 	(640)	
#define LCD_YSIZE_TFT_640480 	(480)

//TFT640480
#define HOZVAL_TFT_640480	(LCD_XSIZE_TFT_640480-1)
#define LINEVAL_TFT_640480	(LCD_YSIZE_TFT_640480-1)

//Timing parameter for V16C6448AB(PRIME VIEW) 
#define VBPD_640480		((33-1)&0xff)
#define VFPD_640480		((10-1)&0xff)
#define VSPW_640480		((2-1) &0x3f)
#define HBPD_640480		((48-1)&0x7f)
#define HFPD_640480		((16-1)&0xff)
#define HSPW_640480		((96-1)&0xff)


#define CLKVAL_TFT_640480 	(1)
    //53.5hz @90Mhz
    //VSYNC,HSYNC should be inverted
    //HBPD=47VCLK,HFPD=15VCLK,HSPW=95VCLK
    //VBPD=32HSYNC,VFPD=9HSYNC,VSPW=1HSYNC

#define LCDFRAMEBUFFER 0x30400000

/*
 * 初始化用于LCD的引脚
 */
void Lcd_Port_Init(void);

/*
 * 初始化LCD控制器
 * 输入参数：
 * type: 显示模式
 *      MODE_TFT_8BIT_640480  : 640*640 8bpp的TFT LCD
 *      MODE_TFT_16BIT_640480 : 640*640 16bpp的TFT LCD
 */
void Tft_Lcd_Init(int type);

/*
 * 设置调色板
 */
void Lcd_Palette8Bit_Init(void);

/*
 * 设置LCD控制器是否输出信号
 * 输入参数：
 * onoff: 
 *      0 : 关闭
 *      1 : 打开
 */
void Lcd_EnvidOnOff(int onoff);

/*
 * 设置是否输出LCD电源开关信号LCD_PWREN
 * 输入参数：
 *     invpwren: 0 - LCD_PWREN有效时为正常极性
 *               1 - LCD_PWREN有效时为反转极性
 *     pwren:    0 - LCD_PWREN输出有效
 *               1 - LCD_PWREN输出无效
 */
void Lcd_PowerEnable(int invpwren, int pwren);

/*
 * 使用临时调色板寄存器输出单色图像
 * 输入参数：
 *     color: 颜色值，格式为0xRRGGBB
 */
void ClearScrWithTmpPlt(UINT32 color);

/*
 * 停止使用临时调色板寄存器
 */
void DisableTmpPlt(void);

/*
 * 改变调色板为一种颜色
 * 输入参数：
 *     color: 颜色值，格式为0xRRGGBB
 */
void ChangePalette(UINT32 color);


#endif /*__LCDDRV_H__*/

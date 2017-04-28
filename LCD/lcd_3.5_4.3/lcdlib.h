/*
 * FILE: lcdlib.h
 * TFT LCD的测试函数接口
 */

#ifndef __LCDLIB_H__
#define __LCDLIB_H__

/* 
 * 以640x480,8bpp的显示模式测试TFT LCD
 */
void Test_Lcd_Tft_8Bit_640480(void);

/* 
 * 以640x480,16bpp的显示模式测试TFT LCD
 */
void Test_Lcd_Tft_16Bit_640480(void);

void Test_Lcd_Tft_16Bit_480272_hceng(void);


#endif /*__LCDLIB_H__*/

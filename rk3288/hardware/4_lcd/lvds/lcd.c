
#include "lcd.h"
#include "printf.h"
#include "timer.h"

static void clk_loop_delayus(unsigned int us)
{
	volatile unsigned int i;
	for (i = 0; i < 100UL * us; i++)
		asm volatile("nop");
}

static void codec_pll_clk_set(void)
{
	//Codec PLL 384MHZ
	CRU_MODE_CON     =  0x03000000; //Codec PLL enter slow-mode
	CRU_CPLL_CON3    =  0x00200020; //Codec PLL reset
	CRU_CPLL_CON0    =  0x3f0f0103; //NO=3+1  NR=1+1
	CRU_CPLL_CON1    =  0x0000007f; //NF=127+1  
	//Codec PLL = ((Fin/NR)*NF)/NO = ((24/2)*128)/4 = 384MHZ
	CRU_CPLL_CON2    =  0x00000040; 
	clk_loop_delayus(5);            //delay 5us
	CRU_CPLL_CON3    =  0x00200000; //return form rest    
	while(!(GRF_SOC_STATUS1 & (0x01<<7))); //waiting for pll lock
	clk_loop_delayus(1);            //delay 1us
	CRU_MODE_CON     =  0x03000100; //Codec PLL enter normal-mode
}

static void lcdc_clk_set(void)
{
	//dclk_vop0 & hclk_vio
	
	//select codec pll clock; 
	//clk_lcdc0=lcdc0_clk_src/(lcdc0_div_con+1) = 384/(5+1) = 64Mhz
	CRU_CLKSEL27_CON = 0xff030500;
	
	//rk3288 eco chip, also set lcdc1 aclk for isp aclk0 and aclk1 should same source
	//VIO0: select codec pll clock; aclk_vio0=vio0_aclk_src/(vio0_aclk_div_con+1) = 64/1 =64Mhz
	//VIO1: select codec pll clock; aclk_vio0=vio0_aclk_src/(vio0_aclk_div_con+1) = 64/1 =64Mhz
	CRU_CLKSEL31_CON = 0xdfdf0000; 
	
	//when set lcdc0, should vio hclk
	//VIO AHB clock divider frequency: clk=clk_src/(div_con+1) = 64/1 =64Mhz
	CRU_CLKSEL28_CON = 0x1f000300;
}


static void lcd_power_enable(void)
{
	//GPIO_A3 底板原理图上未找到,应该是在核心板上的
	GPIO7_SWPORTA_DDR |=  (0x01<<3); 
	GPIO7_SWPORTA_DR  |=  (0x01<<3);	
}

static void lcd_backlight_enable_set(void)
{
	//BL_EN/GPIO5_C1_D  高有效
	//PWM1/GPIO7_A1_D   低有效

	GPIO5_SWPORTA_DDR |=  (0x01<<17); 
	GPIO5_SWPORTA_DR  |=  (0x01<<17);

	GPIO7_SWPORTA_DDR |=  (0x01<<1); //暂时直接输出的低,未使用PWM
	GPIO7_SWPORTA_DR  &= ~(0x01<<1);
}


static void lcdc_init(void)
{
	GRF_IO_VSEL |= (0x01<<16); /*LCDC IO domain 3.3V voltage select io*/
	
	VOP_BIG_SYS_CTRL &= ~(0x0F<<20);
	VOP_BIG_SYS_CTRL |=  (0x01<<20); //enable mmu
	
	VOP_BIG_DSP_CTRL1 &= ~(0xFF<<8);
	VOP_BIG_DSP_CTRL1 |=  (0xE4<<8); //dsp_layer0~4
	
	VOP_BIG_VOP_REG_CFG_DONE |= (0x01<<0); //lcdc_cfg_done
}

static void lcdc_load_screen(void)
{
	//rgb/lvds interface enable
	VOP_BIG_SYS_CTRL &= ~(0xF<<12);
	VOP_BIG_SYS_CTRL |=  (0x1<<12); 
	
	//Cb-Cr filter in CCIR656 mode  1'b0 : drop mode
	//Black /Blank display mode disable
	//dsp_dclk_pol、dsp_den_pol、dsp_vsync_pol、dsp_hsync_pol -> negative
	VOP_BIG_DSP_CTRL0 &= ~((0x01<<20) | (0x01<<19) | (0x01<<18) | (0x01<<7) | (0x01<<6) | (0x01<<5) | (0x01<<4));
	
	//pre_dither_down_en  10bit -> 8bit (allegro)
	//dither_up_en
	VOP_BIG_DSP_CTRL1 &= ~((0x01<<1) | (0x01<<2) | (0x01<<6));
	VOP_BIG_DSP_CTRL1 |=  ((0x01<<1) | (0x01<<6));
	
	//Display delta swap disable; Display dummy swap disable; Hsync/Vsync/Den output software ctrl->normal output
	//Display output format:Parallel 24-bit RGB888 output R[7:0],G[7:0],B[7:0]
	VOP_BIG_DSP_CTRL0 &= ~((0x01<<17) | (0x01<<16) | (0x01<<15) | (0x0F<<0));
	
	//background color 
	VOP_BIG_DSP_BG |= (0x00<<0);   
	
	//这里横竖
	//hspw+hbp+col+hfp:8+8+800+48=848
	VOP_BIG_DSP_HTOTAL_HS_END  =  ((8<<0)   | (848<<16));  //0:HS_PW        16:HTOTAL
	VOP_BIG_DSP_HACT_ST_END    =  ((816<<0) | (16<<16));   //0:COL_END      16:COL_START
	
	//vspw+vbp+row+vfp:4+11+1280+32=1312
	VOP_BIG_DSP_VTOTAL_VS_END  =  ((4<<0)    | (1312<<16));//0:VS_PW        16:VTOTAL
	VOP_BIG_DSP_VACT_ST_END    =  ((1295<<0) | (15<<16));  //0:ROW_END      16:ROW_START
	
	VOP_BIG_POST_DSP_HACT_INFO =  ((816<<0)  | (16<<16));  //0:COL_END      16:COL_START
	VOP_BIG_POST_DSP_VACT_INFO =  ((1295<<0) | (15<<16));  //0:ROW_END      16:ROW_START

	//Panel active horizontal scanning start point and end point
	VOP_BIG_POST_DSP_VACT_INFO_F1 = 0;
	
	VOP_BIG_VOP_POST_RESERVED = 0x10001000;
	
	//MCU mode control register  
	VOP_BIG_MCU_CTRL = 0;
	
	VOP_BIG_INTR_CTRL0 &= ~((0x01<<6) | (0xFFF<<12));
	VOP_BIG_INTR_CTRL0 |= (1295<<12); //ROW_END  
	
	VOP_BIG_VOP_REG_CFG_DONE |= (0x01<<0); //lcdc register config done flag
}


static void lvds_en(void)
{
	 //1'b0: VOP BIG output to LVDS
	GRF_SOC_CON6 |= ((0x01<<3)<<16);

	//LVDS controller enable_1 signal control
	GRF_SOC_CON7  = ((0x01<<11) | ((0x01<<11)<<16)); 
	
	//GPIO1D slew rate control for each bit
	GRF_GPIO1H_SR = 0x0f000f00;
	//GPIO1D drive strength control, every GPIO bit corresponding to 2bits
	GRF_GPIO1D_E  = 0x00ff00ff;
	
	LVDS_CH0_REG_0  = 0xbf;
	LVDS_CH0_REG_40 = 0xbf;
	LVDS_CH0_REG_1  = 0x3f;
	LVDS_CH0_REG_41 = 0x3f;
	LVDS_CH0_REG_2  = 0xfe;
	LVDS_CH0_REG_42 = 0xfe;
	LVDS_CH0_REG_3  = 0x46;
	LVDS_CH0_REG_43 = 0x46;
	LVDS_CH0_REG_4  = 0x00;
	LVDS_CH0_REG_44 = 0x00;
	LVDS_CH0_REG_d  = 0x0a;
	LVDS_CH0_REG_4d = 0x0a;
	LVDS_CH0_REG_20 = 0x44;
	LVDS_CH0_REG_60 = 0x44;
	LVDS_CFG_REG_c  = 0x00;
	LVDS_CFG_REG_21 = 0x92;	
}


unsigned short *fb;
static void fb_data(unsigned short color)
{
	unsigned short i, j;

	fb = (unsigned short *)0x7dc00000; //lcd_base

	for (i = 0; i < 800; ++i) {
		for (j = 0; j < 1280; j++) {
		*fb = color;
		fb ++;	
		}
	}	
}


static lcd_panle_dispaly(void)
{
	/*win0_set_par*/
	VOP_BIG_VOP_WIN0_CTRL0 &= ~((0xFF<<0) | (0x01<<12) | (0x01<<15));
	VOP_BIG_VOP_WIN0_CTRL0 = 0x00000085; //win0_en、RGB565
	
	//win0 vertical scaler down mode select:bilinear
	VOP_BIG_VOP_WIN0_CTRL1 = 0x40400000; 
	
	//Number of words of Win0 Virtual width RGB565 : ceil(win0_vir_width/2)
	VOP_BIG_VOP_WIN0_VIR  = 0x00000064;
	
	//bit[28:26]:win_act_height = (win0 vertical size -1)
	//bit[12:0]: win_act_width = (win0 horizontial size -1)
	VOP_BIG_WIN0_ACT_INFO = ((1281<<16) | (801<<0)); //Win0 active window width/height   
	VOP_BIG_WIN0_DSP_INFO = ((1281<<16) | (801<<0)); //Win0 display width/height on panel   

	//Win0 display start point on panel  
	VOP_BIG_WIN0_DSP_ST   = ((0<<16) | (0<<0));  
	//Win0 YRGB scaling factor  
	VOP_BIG_WIN0_SCL_FACTOR_YRGB = 0x10001000; 
	//Win0 Cbr scaling factor 
	VOP_BIG_WIN0_SCL_FACTOR_CBR  = 0x10001000;
	
	VOP_BIG_WIN0_SRC_ALPHA_CTRL  = 0x00000000;
	
	//Win0 YRGB memory start address 
	VOP_BIG_WIN0_YRGB_MST = 0x7dc00000;


	/*rk_lcdc_set_par*/
	//brightness contrast saturation*contrast config registe
	VOP_BIG_VOP_BCSH_BCS = 0xd0010000;
	//sin hue and cos hue config register  
	VOP_BIG_VOP_BCSH_H   = 0x01000000;
	//color bar config register
	VOP_BIG_BCSH_COLOR_BAR = 1;
	
	if (VOP_BIG_VERSION_INFO == 0x0a050a01)
		VOP_BIG_BCSH_CTRL = 0x11;
	
	VOP_BIG_VOP_REG_CFG_DONE |= (0x01<<0); //lcdc_cfg_done

	VOP_BIG_SYS_CTRL &= ~(0x01<<22); //LCDC standby mode 
}


static unsigned char i2c0_read(unsigned char addr)
{
	unsigned char data = 0;
	
	//0.清空控制寄存器并使能
	I2C0->CON &= ~(0x7F<<0); 
	I2C0->IPD &= ~(0x7F<<0);
	I2C0->CON |= 0x01<<0; //使能
	
	//必须收到ack,否则停止传输(非必需)
	//I2C0->CON |=  (0x01<<6); //stop transaction when NAK handshake is received
	
	//1.设置模式:transmit address (device + register address) --> restart --> transmit address –> receive only
	I2C0->CON |=  (0x01<<1);  //自动发送从机地址和从机寄存器地址
    
	//2.从机地址
	I2C0->MRXADDR = (0xb4 | (1<<24)); 
	
	//3.从机寄存器地址
	I2C0->MRXRADDR = (addr | (1<<24)); //地址只有6位,超过6位怎么办?
	
	//4.开始信号
	I2C0->CON |=  (0x01<<3);
	while(!(I2C0->IPD & (0x01<<4))); 
	I2C0->IPD |=  (0x01<<4); 
	
	//5.接收一个数据且不响应
	I2C0->CON |= (0x01<<5);
	I2C0->MRXCNT = 1;
	while(!(I2C0->IPD & (0x01<<3))); 
	I2C0->IPD |=  (0x01<<3); 
	
	//6.结束信号
	I2C0->CON &= ~(0x01<<3); //手动清除start
	I2C0->CON |= (0x01<<4);
	while(!(I2C0->IPD & (0x01<<5)));
	I2C0->IPD |=  (0x01<<5);
	
	return (I2C0->RXDATA0 & 0xFF);
}

static void i2c0_write(unsigned char addr, unsigned char data)
{
	//0.清空控制寄存器并使能
	I2C0->CON &= ~(0x7F<<0); 
	I2C0->IPD &= ~(0x7F<<0); 
	I2C0->CON |= 0x01<<0; //使能
	
	//1.设置模式:transmit only
	I2C0->CON &= ~(0x03<<1); 
	
	//2.开始信号
	I2C0->CON |= 0x01<<3; //开始信号
	while(!(I2C0->IPD & (0x01<<4))); //等待开始信号发完
	I2C0->IPD |=  (0x01<<4); //清开始信号标志
	
	//3.I2C从机地址+写地址+数据 (3个字节)
	I2C0->TXDATA0 = 0xb4 | (addr<<8) | (data<<16);
	I2C0->MTXCNT = 3; 
	while(!(I2C0->IPD & (0x01<<2))); //MTXCNT data transmit finished interrupt pending bit
	I2C0->IPD |=  (0x01<<2);
	
	//4.结束信号
	I2C0->CON &= ~(0x01<<3); //手动清除start(注意:前面的开始信号控制位理论会自动清0,实测没有,这里必须手动清,否则是开始信号)
	I2C0->CON |= (0x01<<4);  
	while(!(I2C0->IPD & (0x01<<5)));
	I2C0->IPD |=  (0x01<<5);
}

static void pmic_init(void)
{
	//power hold
	GPIO0_SWPORTA_DDR |=  (0x01<<6); 
	GPIO0_SWPORTA_DR  |=  (0x01<<6);

	//I2C0 GPIO init
	PMU_GPIO0_B_IOMUX |= (0x01<<14);
	PMU_GPIO0_C_IOMUX |= (0x01<<0);
	
	PMU_I2C_CLKDIV = 0x00170016;//I2C0 CLK
	
	//if (0xFF == i2c0_read(0x22)) //correct:0xc9
		//printf("act8846 error.\n"); 
	
	i2c0_write(0x10, 0x18);
	i2c0_write(0x12, 0xc9);
	i2c0_write(0x20, 0x39);
	i2c0_write(0x22, 0xc9);
	i2c0_write(0x40, 0x28);
	i2c0_write(0x42, 0xcd);
	i2c0_write(0x58, 0x52);
	i2c0_write(0x59, 0xcd);
	i2c0_write(0x60, 0xf9);
	i2c0_write(0x61, 0xcd);
	i2c0_write(0x68, 0xf9);
	i2c0_write(0x69, 0xcd);
	i2c0_write(0x70, 0xf9);
	i2c0_write(0x71, 0xcd);
	i2c0_write(0x80, 0x54);
	i2c0_write(0x81, 0xcd);
	i2c0_write(0x90, 0x64);
	i2c0_write(0x91, 0xcd);
	i2c0_write(0xa0, 0xe5);
	i2c0_write(0xa1, 0xcd);
}


void lcd_test(void)
{
	int color;
	
	codec_pll_clk_set();
	lcdc_clk_set();
	
	lcd_power_enable();
	lcd_backlight_enable_set();
	
	lcdc_init();
	lcdc_load_screen();
	lvds_en();
	
	fb_data(BLACK);
	lcd_panle_dispaly();
	
	pmic_init();
	
	while(1)
	{
		fb_data(BLACK);
		delay_s(1);
		
		fb_data(RED);
		delay_s(1);
		
		fb_data(GREEN);
		delay_s(1);
		
		fb_data(BLUE);
		delay_s(1);
		
		fb_data(WHITE);
		delay_s(1);
	}
}








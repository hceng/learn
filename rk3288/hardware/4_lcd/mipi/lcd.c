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
	//Codec PLL 350MHZ
	CRU_MODE_CON     =  0x03000000; //Codec PLL enter slow-mode
	CRU_CPLL_CON3    =  0x00200020; //Codec PLL reset
	CRU_CPLL_CON0    =  0x3f0f0105; //NO=5+1  NR=1+1  
	CRU_CPLL_CON1    =  0x1fff00ae; //NF=174+1        
	//Codec PLL = ((Fin/NR)*NF)/NO = ((24/2)*175)/6 = 350MHZ
	CRU_CPLL_CON2    =  0x0fff0057;   
	clk_loop_delayus(5);            //delay 5us
	CRU_CPLL_CON3    =  0x00200000; //return form rest    
	while(!(GRF_SOC_STATUS1 & (0x01<<7))); //waiting for pll lock
	clk_loop_delayus(1);            //delay 1us
	CRU_MODE_CON     =  0x03000100; //Codec PLL enter normal-mode
}

//pll_src = 0, dclk_hz = 70000000, dclk_div = 5
static void lcdc_clk_set(void)
{
	//dclk_vop0 & hclk_vio
	
	//select codec pll clock; 
	//clk_lcdc0=lcdc0_clk_src/(lcdc0_div_con+1) = 350/(4+1) = 70Mhz
	CRU_CLKSEL27_CON = 0xff030400;  //****
	
	//rk3288 eco chip, also set lcdc1 aclk for isp aclk0 and aclk1 should same source
	//VIO0: select codec pll clock; aclk_vio0=vio0_aclk_src/(vio0_aclk_div_con+1) = 70/1 =70Mhz
	//VIO1: select codec pll clock; aclk_vio0=vio0_aclk_src/(vio0_aclk_div_con+1) = 70/1 =70Mhz
	CRU_CLKSEL31_CON = 0xdfdf0000; 
	
	//when set lcdc0, should vio hclk
	//VIO AHB clock divider frequency: clk=clk_src/(div_con+1) = 70/1 =70Mhz
	CRU_CLKSEL28_CON = 0x1f000300;
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


static void rockchip_vop_init(void)
{
	VOP_BIG_DSP_CTRL0 |= (0x01<<7); //DCLK invert enable
	VOP_BIG_SYS_CTRL  |= ((0x01<<15) | (0x01<<23)); //mipi interface enable;enable auto gating
	
	VOP_BIG_DSP_CTRL1 |= (0x01<<1); //10bit -> 8bit (allegro); dsp_layer0~4
	
	//VOP_BIG_BCSH_CTRL = 0x00000000; //VOP_BCSH_RESERVED
	//VOP_BIG_BCSH_CTRL = 0x00000040;
	//VOP_BIG_BCSH_CTRL = 0x00000044;
	
	VOP_BIG_DSP_BG = 0x00; //background color
	
	//hspw+hbp+col+hfp:4+40+800+40=884
	VOP_BIG_DSP_HTOTAL_HS_END  =  ((4<<0)   | (884<<16));  //0:HS_PW        16:HTOTAL
	VOP_BIG_DSP_HACT_ST_END    =  ((844<<0) | (44<<16));   //0:COL_END      16:COL_START
	
	//vspw+vbp+row+vfp:2+22+1280+16=1320
	VOP_BIG_DSP_VTOTAL_VS_END  =  ((2<<0)    | (1320<<16));//0:VS_PW        16:VTOTAL
	VOP_BIG_DSP_VACT_ST_END    =  ((1304<<0) | (24<<16));  //0:ROW_END      16:ROW_START
	
	VOP_BIG_POST_DSP_HACT_INFO =  ((844<<0)  | (44<<16));  //0:COL_END      16:COL_START
	VOP_BIG_POST_DSP_VACT_INFO =  ((1304<<0) | (24<<16));  //0:ROW_END      16:ROW_START	
	
	VOP_BIG_POST_SCL_FACTOR_YRGB = 0x10001000; //post yrgb scaling factor   //hceng:???
	VOP_BIG_POST_SCL_CTRL = 0x00; //post scaling start point offset(disable)
	
	VOP_BIG_SYS_CTRL |= (0x01<<22);  //LCDC standby mode enable(stop output)
	VOP_BIG_INTR_CTRL0 |= (1301<<12); //Line number of the Line flag interrupt(0~VTOTAL-1)  //hceng:感觉应该是2+22+1280=1304,测试下其它值
	
	VOP_BIG_VOP_REG_CFG_DONE |= (0x01<<0); //lcdc register config done flag
}

static void display_set_plane(void)
{
	//rockchip_vop_set_plane
	
	VOP_BIG_VOP_WIN0_CTRL0 = ((0x02<<1) | (0x02<<5)); //RGB565;win0_lb_mode
	VOP_BIG_VOP_WIN0_VIR   = 0x01400122; //Number of words of Win0 Virtual width
	
	VOP_BIG_WIN0_YRGB_MST  = 0x7dc00000; //fb_addr
	VOP_BIG_VOP_WIN0_CTRL0 = ((0x02<<1) | (0x04<<5)); //RGB565;win0_lb_mode
	
	VOP_BIG_WIN0_SCL_FACTOR_YRGB = 0x10001000; //Win0 YRGB scaling factor 
	//VOP_BIG_VOP_WIN0_CTRL1 = 0x00000000; //default
	
	VOP_BIG_WIN0_ACT_INFO = ((1279<<16) | (799<<0)); //Win0 active window width/height
	VOP_BIG_WIN0_DSP_INFO = ((1279<<16) | (799<<0));; //Win0 display width/height on panel  
	
	VOP_BIG_WIN0_DSP_ST    = ((0<<16) | (0<<0)); //Win0 display start point on panel 
	VOP_BIG_VOP_WIN0_CTRL0 = ((0x02<<1) | (0x04<<5) | (0x01<<12)); //RGB565;win0_lb_mode;win0 RGB RED and BLUE swap(BGR)
	
	VOP_BIG_WIN0_SRC_ALPHA_CTRL = 0x00000000; //default
	VOP_BIG_VOP_WIN0_CTRL0 = ((0x02<<1) | (0x04<<5) | (0x01<<12) | (0x01<<0)); //win0_en
	
	VOP_BIG_VOP_REG_CFG_DONE |= (0x01<<0); //lcdc register config done flag
}

static void rockchip_dsi_panel_send_cmds(void)
{
	GPIO7_SWPORTA_DDR |=  (0x01<<2);  //BL_EN/GPIO7_A2_D     高有效
	GPIO7_SWPORTA_DR  |=  (0x01<<2);

	GPIO7_SWPORTA_DDR |=  (0x01<<3);  //LCD_RST/GPIO7_A3_D   高有效
	GPIO7_SWPORTA_DR  &= ~(0x01<<3);
	GPIO7_SWPORTA_DR  |=  (0x01<<3);
	
	//rockchip_set_transfer_mode
	DSI0->CMD_MODE_CFG = 0x010f7f00; //Low-power mode
	DSI0->LPCLK_CTRL = 0x00000000; 
	//rockchip_dsi_send_packet
	clk_loop_delayus(5000);
	DSI0->GEN_HDR = 0x00001105;
	
	//rockchip_set_transfer_mode
	DSI0->CMD_MODE_CFG = 0x010f7f00; 
	DSI0->LPCLK_CTRL = 0x00000000; 
	//rockchip_dsi_send_packet
	clk_loop_delayus(5000);
	DSI0->GEN_HDR = 0x00002905;
	
	clk_loop_delayus(5000);	
}


static void rockchip_vop_enable(void)
{
	VOP_BIG_SYS_CTRL &= ~(0x01<<22); //LCDC standby mode(disable)
	VOP_BIG_VOP_REG_CFG_DONE |= (0x01<<0); //lcdc register config done flag
}

static void rockchip_dw_mipi_dsi_enable(void)
{
	//dw_mipi_dsi_set_mode(dsi, DSI_VIDEO_MODE);
	DSI0->PWR_UP     = 0x00000000;
	DSI0->LPCLK_CTRL = 0x00000001; //phy_txrequestclkhs
	DSI0->MODE_CFG   = 0x00000000;
	DSI0->PWR_UP     = 0x00000001; //Power-up
}


static void rockchip_panel_enable(void)
{
	GRF_GPIO7A_IOMUX= 0xffff0000;

	GPIO7_SWPORTA_DDR |=  (0x01<<0); //PWM
	GPIO7_SWPORTA_DR  |=  (0x01<<0);
	
}


static void dw_mipi_dsi_phy_write(unsigned char code, unsigned char data)
{
	DSI0->PHY_TST_CTRL0 = 0x00000002;
	DSI0->PHY_TST_CTRL_1 = ((code&0xFF) | (1<<16));
	DSI0->PHY_TST_CTRL0 = 0x00000000;
	DSI0->PHY_TST_CTRL_1 = (data&0xFF);
	DSI0->PHY_TST_CTRL0 = 0x00000002;
}

static void display_enable(void)
{
	GRF_SOC_CON6 = 0x00400000; //VOP BIG output to DSI host0

	DSI0->CLKMGR_CFG = ((0x0A<<8) | (0x07<<0)); //Configuration of the internal clock dividers 
	DSI0->DPI_COLOR_CODING = 0x00000005; //这里的DPI_COLOR_CODING在芯片手册对应操作位是没有介绍的,但却和C0_DPI_COLOR_CODING对应,但直接操作C0_DPI_COLOR_CODING不行
	DSI0->MODE_CFG = 0x00000006; //dsi_set_mode？
	DSI0->LP_CMD_TIM = 0x00040004; //和C0_LP_CMD_TIM对应
	DSI0->PCKHDL_CFG = 0x0000001c; //Packet handler configuration(enable)
	DSI0->VID_MODE_CFG = 0x00002f02; //Video mode configuration. 
	DSI0->VID_PKT_SIZE = 0x00000320; //video_packet_config
	DSI0->TO_CNT_CFG = 0x03e803e8; //Timeout timers configuration 
	DSI0->BTA_TO_CNT = 0x00000d00; //bta_to_cnt

	DSI0->MODE_CFG = 0x00000001;//dsi_set_mode 
	DSI0->VID_HLINE_TIME = 0x0000062b; //hline_time //和理论值不一样？
	DSI0->VID_HSA_TIME   = 0x00000007; //hsa_time 
	DSI0->VID_HBP_TIME   = 0x00000047; //hbp_time

	DSI0->VID_VACTIVE_LINES = 0x00000500; //v_active_line
	DSI0->VID_VSA_LINES     = 0x00000002; //vsa_lines
	DSI0->VID_VBP_LINES     = 0x00000016; //vbp_lines
	DSI0->VID_VFP_LINES     = 0x00000010; //vfp_lines
	
	DSI0->PHY_TMR_CFG = 0x14102710; //D-PHY timing configuration
	DSI0->PHY_TMR_LPCLK_CFG = 0x00400040; //phy_lp2hs_time  phy_hs2lp_time 

	DSI0->PHY_IF_CFG = 0x00002003; //D-PHY interface configuration(Four data lanes(lanes 0,1,2,and 3) )
		
	DSI0->MSK0 = 0x00000000; //Masks the interrupt generation triggerd by the ERROR_ST0 reg
	DSI0->MSK1 = 0x00000000; //Masks the interrupt generation triggerd by the ERROR_ST1s reg
	
	GRF_SOC_CON15 = 0x07000000; //[10:8]MIPI DPHY TX0 turn around request
	GRF_SOC_CON8  = 0x0fff0000; //[11:8] [7:4] [3:0]

	//dw_mipi_dsi_phy_init
	DSI0->PWR_UP = 0x00000001;
	dw_mipi_dsi_phy_write(0x10, 0xab);
	dw_mipi_dsi_phy_write(0x11, 0x08);
	dw_mipi_dsi_phy_write(0x12, 0xc0);
	dw_mipi_dsi_phy_write(0x44, 0x54);
	dw_mipi_dsi_phy_write(0x17, 0x02);
	dw_mipi_dsi_phy_write(0x18, 0x1c);
	dw_mipi_dsi_phy_write(0x19, 0x30);
	dw_mipi_dsi_phy_write(0x18, 0x83);
	dw_mipi_dsi_phy_write(0x19, 0x30);
	dw_mipi_dsi_phy_write(0x20, 0x4d);
	dw_mipi_dsi_phy_write(0x21, 0x3d);
	dw_mipi_dsi_phy_write(0x21, 0xdf);
	dw_mipi_dsi_phy_write(0x22, 0x07);
	dw_mipi_dsi_phy_write(0x22, 0x87);
	dw_mipi_dsi_phy_write(0x70, 0x8f);
	dw_mipi_dsi_phy_write(0x71, 0xad);
	dw_mipi_dsi_phy_write(0x72, 0x4a);
		
	DSI0->PHY_RSTZ = 0x0000000f; //D-PHY reset control
	
	rockchip_dsi_panel_send_cmds();
	rockchip_vop_enable();
	rockchip_dw_mipi_dsi_enable();
	rockchip_panel_enable();
}


void lcd_test(void)
{
	int color;
	
	pmic_init();
		
	codec_pll_clk_set();
	lcdc_clk_set();
	rockchip_vop_init();
	
	fb_data(BLACK);
		
	display_set_plane();
	display_enable();
	
	while(1)
	{
		fb_data(RED);
		delay_s(1);
		
		fb_data(GREEN);
		delay_s(1);
		
		fb_data(BLUE);
		delay_s(1);
		
		fb_data(WHITE);
		delay_s(1);
		
		fb_data(BLACK);
		delay_s(1);
	}
}




















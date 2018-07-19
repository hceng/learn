#ifndef   __LCD_H__
#define   __LCD_H__

#define    I2C_PMU             0xFF650000
#define    PMU_I2C_CLKDIV      (*(volatile unsigned long *)(I2C_PMU + 0x004))

typedef volatile struct i2c_reg {
	unsigned long CON;        //0x0000 control register
	unsigned long CLKDIV;     //0x0004 Clock divisor register
	unsigned long MRXADDR;	  //0x0008 the slave address accessed for master receive mode
	unsigned long MRXRADDR;   //0x000C the slave register address accessed for master recevie mode
	unsigned long MTXCNT;     //0x0010 master transmit count
	unsigned long MRXCNT;     //0x0014 master receive count
	unsigned long IEN;        //0x0018 interrupt enable register
	unsigned long IPD;        //0x001C interrupt pending register
	unsigned long FCNT;       //0x0020 finished count
	unsigned long RESERVED0[55];
	unsigned long TXDATA0;    //0x0100 I2C transmit data register 0
	unsigned long TXDATA1;    //0x0104
	unsigned long TXDATA2;    //0x0108
	unsigned long TXDATA3;    //0x010C
	unsigned long TXDATA4;    //0x0110
	unsigned long TXDATA5;    //0x0114
	unsigned long TXDATA6;    //0x0118
	unsigned long TXDATA7;	  //0x011C
	unsigned long RESERVED1[56];
	unsigned long RXDATA0;	  //0x0200 I2C receive data register 0
	unsigned long RXDATA1;	  //0x0204
	unsigned long RXDATA2;    //0x0208
	unsigned long RXDATA3;	  //0x020C
	unsigned long RXDATA4;    //0x0210
	unsigned long RXDATA5;    //0x0214
	unsigned long RXDATA6;    //0x0218
	unsigned long RXDATA7;    //0x021C
} i2c_reg, *p_i2c_reg;

#define I2C0      ((p_i2c_reg)I2C_PMU)

//-----------------------------PMU----------------------------------------//
#define    PMU                 0xFF730000
#define    PMU_GPIO0_B_IOMUX   (*(volatile unsigned long *)(PMU + 0x0088))   
#define    PMU_GPIO0_C_IOMUX   (*(volatile unsigned long *)(PMU + 0x008C))  

//-----------------------------LVDS----------------------------------------//
#define    LVDS               0xFF96C000      
#define    LVDS_CH0_REG_0     (*(volatile unsigned long *)(LVDS + 0x0000))          //P1263
#define    LVDS_CH0_REG_1     (*(volatile unsigned long *)(LVDS + 0x0004))          
#define    LVDS_CH0_REG_2     (*(volatile unsigned long *)(LVDS + 0x0008))          
#define    LVDS_CH0_REG_3     (*(volatile unsigned long *)(LVDS + 0x000c))          
#define    LVDS_CH0_REG_4     (*(volatile unsigned long *)(LVDS + 0x0010))          
#define    LVDS_CH0_REG_5     (*(volatile unsigned long *)(LVDS + 0x0014))          
//#define    LVDS_CH0_REG_9     (*(volatile unsigned long *)(LVDS + 0x0024))          
#define    LVDS_CFG_REG_c     (*(volatile unsigned long *)(LVDS + 0x0030))          
#define    LVDS_CH0_REG_d     (*(volatile unsigned long *)(LVDS + 0x0034))   
#define    LVDS_CH0_REG_f     (*(volatile unsigned long *)(LVDS + 0x003c))   
#define    LVDS_CH0_REG_20    (*(volatile unsigned long *)(LVDS + 0x0080))   
#define    LVDS_CFG_REG_21    (*(volatile unsigned long *)(LVDS + 0x0084))   

#define    LVDS_CH0_REG_40    (*(volatile unsigned long *)(LVDS + 0x0100)) 
#define    LVDS_CH0_REG_41    (*(volatile unsigned long *)(LVDS + 0x0104))          
#define    LVDS_CH0_REG_42    (*(volatile unsigned long *)(LVDS + 0x0108))          
#define    LVDS_CH0_REG_43    (*(volatile unsigned long *)(LVDS + 0x010c))          
#define    LVDS_CH0_REG_44    (*(volatile unsigned long *)(LVDS + 0x0110))          
#define    LVDS_CH0_REG_45    (*(volatile unsigned long *)(LVDS + 0x0114))          
//#define    LVDS_CH0_REG_49    (*(volatile unsigned long *)(LVDS + 0x0124))          
//#define    LVDS_CFG_REG_4c    (*(volatile unsigned long *)(LVDS + 0x0130))          
#define    LVDS_CH0_REG_4d    (*(volatile unsigned long *)(LVDS + 0x0134)) 
  
#define    LVDS_CH0_REG_60    (*(volatile unsigned long *)(LVDS + 0x0180))                             

//-----------------------------GRF----------------------------------------//
#define    GRF                0xFF770000                                   
#define    GRF_GPIO1H_SR      (*(volatile unsigned long *)(GRF + 0x0104)) 
#define    GRF_GPIO1D_E       (*(volatile unsigned long *)(GRF + 0x01CC)) 
#define    GRF_SOC_CON6       (*(volatile unsigned long *)(GRF + 0x025C)) 
#define    GRF_SOC_CON7       (*(volatile unsigned long *)(GRF + 0x0260))  
#define    GRF_SOC_STATUS1    (*(volatile unsigned long *)(GRF + 0x0284))
#define    GRF_IO_VSEL        (*(volatile unsigned long *)(GRF + 0x0380))  
//-----------------------------CRU----------------------------------------//
#define    CRU                0xFF760000                                   

#define    CRU_APLL_CON0      (*(volatile unsigned long *)(CRU + 0x0000))
#define    CRU_APLL_CON1      (*(volatile unsigned long *)(CRU + 0x0004))
#define    CRU_APLL_CON2      (*(volatile unsigned long *)(CRU + 0x0008))
#define    CRU_APLL_CON3      (*(volatile unsigned long *)(CRU + 0x000C))
#define    CRU_DPLL_CON0      (*(volatile unsigned long *)(CRU + 0x0010))
#define    CRU_DPLL_CON1      (*(volatile unsigned long *)(CRU + 0x0014))
#define    CRU_DPLL_CON2      (*(volatile unsigned long *)(CRU + 0x0018))
#define    CRU_DPLL_CON3      (*(volatile unsigned long *)(CRU + 0x001C))
#define    CRU_CPLL_CON0      (*(volatile unsigned long *)(CRU + 0x0020))
#define    CRU_CPLL_CON1      (*(volatile unsigned long *)(CRU + 0x0024))
#define    CRU_CPLL_CON2      (*(volatile unsigned long *)(CRU + 0x0028))
#define    CRU_CPLL_CON3      (*(volatile unsigned long *)(CRU + 0x002C))
#define    CRU_GPLL_CON0      (*(volatile unsigned long *)(CRU + 0x0030))
#define    CRU_GPLL_CON1      (*(volatile unsigned long *)(CRU + 0x0034))
#define    CRU_GPLL_CON2      (*(volatile unsigned long *)(CRU + 0x0038))
#define    CRU_GPLL_CON3      (*(volatile unsigned long *)(CRU + 0x003C))
#define    CRU_NPLL_CON0      (*(volatile unsigned long *)(CRU + 0x0040))
#define    CRU_NPLL_CON1      (*(volatile unsigned long *)(CRU + 0x0044))
#define    CRU_NPLL_CON2      (*(volatile unsigned long *)(CRU + 0x0048))
#define    CRU_NPLL_CON3      (*(volatile unsigned long *)(CRU + 0x004C))

#define    CRU_MODE_CON       (*(volatile unsigned long *)(CRU + 0x0050))

#define    CRU_CLKSEL0_CON    (*(volatile unsigned long *)(CRU + 0x0060))
#define    CRU_CLKSEL1_CON    (*(volatile unsigned long *)(CRU + 0x0064))
#define    CRU_CLKSEL2_CON    (*(volatile unsigned long *)(CRU + 0x0068))
#define    CRU_CLKSEL3_CON    (*(volatile unsigned long *)(CRU + 0x006C))
#define    CRU_CLKSEL4_CON    (*(volatile unsigned long *)(CRU + 0x0070))
#define    CRU_CLKSEL5_CON    (*(volatile unsigned long *)(CRU + 0x0074))
#define    CRU_CLKSEL6_CON    (*(volatile unsigned long *)(CRU + 0x0078))
#define    CRU_CLKSEL7_CON    (*(volatile unsigned long *)(CRU + 0x007C))
#define    CRU_CLKSEL8_CON    (*(volatile unsigned long *)(CRU + 0x0080))
#define    CRU_CLKSEL9_CON    (*(volatile unsigned long *)(CRU + 0x0084))
#define    CRU_CLKSEL10_CON   (*(volatile unsigned long *)(CRU + 0x0088))
#define    CRU_CLKSEL11_CON   (*(volatile unsigned long *)(CRU + 0x008C))
#define    CRU_CLKSEL12_CON   (*(volatile unsigned long *)(CRU + 0x0090))
#define    CRU_CLKSEL13_CON   (*(volatile unsigned long *)(CRU + 0x0094))
#define    CRU_CLKSEL14_CON   (*(volatile unsigned long *)(CRU + 0x0098))
#define    CRU_CLKSEL15_CON   (*(volatile unsigned long *)(CRU + 0x009C))
#define    CRU_CLKSEL16_CON   (*(volatile unsigned long *)(CRU + 0x00A0))
#define    CRU_CLKSEL17_CON   (*(volatile unsigned long *)(CRU + 0x00A4))
#define    CRU_CLKSEL18_CON   (*(volatile unsigned long *)(CRU + 0x00A8))
#define    CRU_CLKSEL19_CON   (*(volatile unsigned long *)(CRU + 0x00AC))
#define    CRU_CLKSEL20_CON   (*(volatile unsigned long *)(CRU + 0x00B0))
#define    CRU_CLKSEL21_CON   (*(volatile unsigned long *)(CRU + 0x00B4))
#define    CRU_CLKSEL22_CON   (*(volatile unsigned long *)(CRU + 0x00B8))
#define    CRU_CLKSEL23_CON   (*(volatile unsigned long *)(CRU + 0x00BC))
#define    CRU_CLKSEL24_CON   (*(volatile unsigned long *)(CRU + 0x00C0))
#define    CRU_CLKSEL25_CON   (*(volatile unsigned long *)(CRU + 0x00C4))
#define    CRU_CLKSEL26_CON   (*(volatile unsigned long *)(CRU + 0x00C8))
#define    CRU_CLKSEL27_CON   (*(volatile unsigned long *)(CRU + 0x00CC))
#define    CRU_CLKSEL28_CON   (*(volatile unsigned long *)(CRU + 0x00D0))
#define    CRU_CLKSEL29_CON   (*(volatile unsigned long *)(CRU + 0x00D4))
#define    CRU_CLKSEL30_CON   (*(volatile unsigned long *)(CRU + 0x00D8))
#define    CRU_CLKSEL31_CON   (*(volatile unsigned long *)(CRU + 0x00DC))
#define    CRU_CLKSEL32_CON   (*(volatile unsigned long *)(CRU + 0x00E0))
#define    CRU_CLKSEL33_CON   (*(volatile unsigned long *)(CRU + 0x00E4))
#define    CRU_CLKSEL34_CON   (*(volatile unsigned long *)(CRU + 0x00E8))
#define    CRU_CLKSEL35_CON   (*(volatile unsigned long *)(CRU + 0x00EC))
#define    CRU_CLKSEL36_CON   (*(volatile unsigned long *)(CRU + 0x00F0))
#define    CRU_CLKSEL37_CON   (*(volatile unsigned long *)(CRU + 0x00F4))
#define    CRU_CLKSEL38_CON   (*(volatile unsigned long *)(CRU + 0x00F8))
#define    CRU_CLKSEL39_CON   (*(volatile unsigned long *)(CRU + 0x00FC))
#define    CRU_CLKSEL40_CON   (*(volatile unsigned long *)(CRU + 0x0100))
#define    CRU_CLKSEL41_CON   (*(volatile unsigned long *)(CRU + 0x0104))
#define    CRU_CLKSEL42_CON   (*(volatile unsigned long *)(CRU + 0x0108))

#define    CRU_CLKGATE0_CON   (*(volatile unsigned long *)(CRU + 0x0160))
#define    CRU_CLKGATE1_CON   (*(volatile unsigned long *)(CRU + 0x0164))
#define    CRU_CLKGATE2_CON   (*(volatile unsigned long *)(CRU + 0x0168))
#define    CRU_CLKGATE3_CON   (*(volatile unsigned long *)(CRU + 0x016C))
#define    CRU_CLKGATE4_CON   (*(volatile unsigned long *)(CRU + 0x0170))
#define    CRU_CLKGATE5_CON   (*(volatile unsigned long *)(CRU + 0x0174))
#define    CRU_CLKGATE6_CON   (*(volatile unsigned long *)(CRU + 0x0178))
#define    CRU_CLKGATE7_CON   (*(volatile unsigned long *)(CRU + 0x017C))
#define    CRU_CLKGATE8_CON   (*(volatile unsigned long *)(CRU + 0x0180))
#define    CRU_CLKGATE9_CON   (*(volatile unsigned long *)(CRU + 0x0184))
#define    CRU_CLKGATE10_CON  (*(volatile unsigned long *)(CRU + 0x0188))
#define    CRU_CLKGATE11_CON  (*(volatile unsigned long *)(CRU + 0x018C))
#define    CRU_CLKGATE12_CON  (*(volatile unsigned long *)(CRU + 0x0190))
#define    CRU_CLKGATE13_CON  (*(volatile unsigned long *)(CRU + 0x0194))
#define    CRU_CLKGATE14_CON  (*(volatile unsigned long *)(CRU + 0x0198))
#define    CRU_CLKGATE15_CON  (*(volatile unsigned long *)(CRU + 0x019C))
#define    CRU_CLKGATE16_CON  (*(volatile unsigned long *)(CRU + 0x01A0))
#define    CRU_CLKGATE17_CON  (*(volatile unsigned long *)(CRU + 0x01A4))
#define    CRU_CLKGATE18_CON  (*(volatile unsigned long *)(CRU + 0x01A8))

#define    CRU_GLB_SRST_FST_VALUE  (*(volatile unsigned long *)(CRU + 0x01b0))
#define    CRU_GLB_SRST_SND_VALUE  (*(volatile unsigned long *)(CRU + 0x01b4))

#define    CRU_SOFTRST0_CON    (*(volatile unsigned long *)(CRU + 0x01b8))
#define    CRU_SOFTRST1_CON    (*(volatile unsigned long *)(CRU + 0x01BC))
#define    CRU_SOFTRST2_CON    (*(volatile unsigned long *)(CRU + 0x01C0))
#define    CRU_SOFTRST3_CON    (*(volatile unsigned long *)(CRU + 0x01C4))
#define    CRU_SOFTRST4_CON    (*(volatile unsigned long *)(CRU + 0x01C8))
#define    CRU_SOFTRST5_CON    (*(volatile unsigned long *)(CRU + 0x01CC))
#define    CRU_SOFTRST6_CON    (*(volatile unsigned long *)(CRU + 0x01D0))
#define    CRU_SOFTRST7_CON    (*(volatile unsigned long *)(CRU + 0x01D4))
#define    CRU_SOFTRST8_CON    (*(volatile unsigned long *)(CRU + 0x01D8))
#define    CRU_SOFTRST9_CON    (*(volatile unsigned long *)(CRU + 0x01DC))
#define    CRU_SOFTRST10_CON   (*(volatile unsigned long *)(CRU + 0x01E0))
#define    CRU_SOFTRST11_CON   (*(volatile unsigned long *)(CRU + 0x01E4))

#define    CRU_MISC_CON        (*(volatile unsigned long *)(CRU + 0x01E8))
#define    CRU_GLB_CNT_TH      (*(volatile unsigned long *)(CRU + 0x01EC))
#define    CRU_GLB_RST_CON     (*(volatile unsigned long *)(CRU + 0x01F0))
#define    CRU_GLB_RST_ST      (*(volatile unsigned long *)(CRU + 0x01F8))

#define    CRU_SDMMC_CON0      (*(volatile unsigned long *)(CRU + 0x0200))
#define    CRU_SDMMC_CON1      (*(volatile unsigned long *)(CRU + 0x0204))
#define    CRU_SDIO0_CON0      (*(volatile unsigned long *)(CRU + 0x0208))
#define    CRU_SDIO0_CON1      (*(volatile unsigned long *)(CRU + 0x020C))
#define    CRU_SDIO1_CON0      (*(volatile unsigned long *)(CRU + 0x0210))
#define    CRU_SDIO1_CON1      (*(volatile unsigned long *)(CRU + 0x0214))
#define    CRU_EMMC_CON0       (*(volatile unsigned long *)(CRU + 0x0218))
#define    CRU_EMMC_CON1       (*(volatile unsigned long *)(CRU + 0x021C))

//-----------------------------GPIO---------------------------------------//
#define    GPIO0              0xFF750000                                         
#define    GPIO0_SWPORTA_DR   (*(volatile unsigned long *)(GPIO0 + 0x04*0))       
#define    GPIO0_SWPORTA_DDR  (*(volatile unsigned long *)(GPIO0 + 0x04*1))  

#define    GPIO5              0xFF7C0000                                         
#define    GPIO5_SWPORTA_DR   (*(volatile unsigned long *)(GPIO5 + 0x04*0))       
#define    GPIO5_SWPORTA_DDR  (*(volatile unsigned long *)(GPIO5 + 0x04*1))        

#define    GPIO7              0xFF7E0000                                         
#define    GPIO7_SWPORTA_DR   (*(volatile unsigned long *)(GPIO7 + 0x04*0))       
#define    GPIO7_SWPORTA_DDR  (*(volatile unsigned long *)(GPIO7 + 0x04*1))    
//------------------------------VOP---------------------------------------//
#define    VOP_BIG             0xFF930000 
#define    VOP_BIG_VOP_REG_CFG_DONE       (*(volatile unsigned long *)(VOP_BIG + 0x0000)) 
#define    VOP_BIG_VERSION_INFO           (*(volatile unsigned long *)(VOP_BIG + 0x0004))   								   
#define    VOP_BIG_SYS_CTRL               (*(volatile unsigned long *)(VOP_BIG + 0x0008))   
#define    VOP_BIG_DSP_CTRL0              (*(volatile unsigned long *)(VOP_BIG + 0x0010)) 
#define    VOP_BIG_DSP_CTRL1              (*(volatile unsigned long *)(VOP_BIG + 0x0014)) 
#define    VOP_BIG_DSP_BG                 (*(volatile unsigned long *)(VOP_BIG + 0x0018)) 
#define    VOP_BIG_MCU_CTRL               (*(volatile unsigned long *)(VOP_BIG + 0x001C)) 
#define    VOP_BIG_INTR_CTRL0             (*(volatile unsigned long *)(VOP_BIG + 0x0020)) 
#define    VOP_BIG_VOP_WIN0_CTRL0         (*(volatile unsigned long *)(VOP_BIG + 0x0030)) 
#define    VOP_BIG_VOP_WIN0_CTRL1         (*(volatile unsigned long *)(VOP_BIG + 0x0034))
#define    VOP_BIG_VOP_WIN0_VIR           (*(volatile unsigned long *)(VOP_BIG + 0x003C))
#define    VOP_BIG_WIN0_YRGB_MST          (*(volatile unsigned long *)(VOP_BIG + 0x0040))
#define    VOP_BIG_WIN0_ACT_INFO          (*(volatile unsigned long *)(VOP_BIG + 0x0048))
#define    VOP_BIG_WIN0_DSP_INFO          (*(volatile unsigned long *)(VOP_BIG + 0x004C))
#define    VOP_BIG_WIN0_DSP_ST            (*(volatile unsigned long *)(VOP_BIG + 0x0050))
#define    VOP_BIG_WIN0_SCL_FACTOR_YRGB   (*(volatile unsigned long *)(VOP_BIG + 0x0054))
#define    VOP_BIG_WIN0_SCL_FACTOR_CBR    (*(volatile unsigned long *)(VOP_BIG + 0x0058))
#define    VOP_BIG_WIN0_SRC_ALPHA_CTRL    (*(volatile unsigned long *)(VOP_BIG + 0x0060))

#define    VOP_BIG_DSP_HTOTAL_HS_END      (*(volatile unsigned long *)(VOP_BIG + 0x0188)) 
#define    VOP_BIG_DSP_HACT_ST_END        (*(volatile unsigned long *)(VOP_BIG + 0x018C)) 
#define    VOP_BIG_DSP_VTOTAL_VS_END      (*(volatile unsigned long *)(VOP_BIG + 0x0190)) 
#define    VOP_BIG_DSP_VACT_ST_END        (*(volatile unsigned long *)(VOP_BIG + 0x0194)) 
#define    VOP_BIG_POST_DSP_HACT_INFO     (*(volatile unsigned long *)(VOP_BIG + 0x0170)) 
#define    VOP_BIG_POST_DSP_VACT_INFO     (*(volatile unsigned long *)(VOP_BIG + 0x0174)) 
#define    VOP_BIG_POST_DSP_VACT_INFO_F1  (*(volatile unsigned long *)(VOP_BIG + 0x0184)) 
#define    VOP_BIG_VOP_POST_RESERVED      (*(volatile unsigned long *)(VOP_BIG + 0x017C)) 

#define    VOP_BIG_BCSH_COLOR_BAR         (*(volatile unsigned long *)(VOP_BIG + 0x01B0))
#define    VOP_BIG_VOP_BCSH_BCS           (*(volatile unsigned long *)(VOP_BIG + 0x01B4)) 
#define    VOP_BIG_VOP_BCSH_H             (*(volatile unsigned long *)(VOP_BIG + 0x01B8)) 
#define    VOP_BIG_BCSH_CTRL              (*(volatile unsigned long *)(VOP_BIG + 0x01BC)) 

//-----------------------------COLOR_RGB565---------------------------------------//
#define   BLACK     0x0000          //黑色    0,   0,   0
#define   NAVY      0x000F          //深蓝色  0,   0, 127
#define   DGREEN    0x03E0          //深绿色  0,  127,  0
#define   DCYAN     0x03EF          //深青色  0,  127, 127       
#define   MAROON    0x7800          //深红色  127,   0,   0      
#define   PURPLE    0x780F          //紫色    127,   0, 127      
#define   OLIVE     0x7BE0          //橄榄绿  127, 127,   0      
#define   LGRAY     0xC618          //灰白色  192, 192, 192      
#define   DGRAY     0x7BEF          //深灰色  127, 127, 127      
#define   BLUE      0x001F          //蓝色    0,   0, 255        
#define   GREEN     0x07E0          //绿色    0, 255,   0        
#define   CYAN      0x07FF          //青色    0, 255, 255        
#define   RED       0xF800          //红色    255,   0,   0      
#define   MAGENTA   0xF81F          //品红    255,   0, 255      
#define   YELLOW    0xFFE0          //黄色    255, 255, 0        
#define   WHITE     0xFFFF          //白色    255, 255, 255 

#endif
#ifndef   __INT_H__
#define   __INT_H__

#define    CORE_GIC           0xFFC00000                                     //P37

//DDI0471A_gic400_r0p0_trm.pdf P33
/**********************************************************************************************************
* Address range           GIC-400 functional block
* 0x0000-0x0FFF           Reserved
* 0x1000-0x1FFF           Distributor
* 0x2000-0x3FFF           CPU interfaces
* 0x4000-0x4FFF           Virtual interface control block, for the processor that is performing the access
* 0x5000-0x5FFF           Virtual interface control block, for the processor selected by address bits [11:9]
* 	0x5000-0x51FF         Alias for Processor 0
* 	0x5200-0x53FF  	      Alias for Processor 1
* 	...                   ...
* 	0x5E00-0x5FFF 		  Alias for Processor 7
* 0x6000-0x7FFF           Virtual CPU interfaces
**********************************************************************************************************/
#define    GICD               (CORE_GIC + 0x1000) //Distributor register map
#define    GICC               (CORE_GIC + 0x2000) //CPU interface register map

//arm_gic_architecture_specification.pdf
//Distributor register map
typedef volatile struct gicd_reg {
	unsigned long icddcr;      	    //0x000
	unsigned long icdictr;    	    //0x004
	unsigned long icdiidr;      	//0x008
	unsigned long reserved0[29];
	unsigned long icdisr[4];	    //0x080
	unsigned long reserved1[28];
	unsigned long icdiser[4];	    //0x100
	unsigned long reserved2[28];
	unsigned long icdicer[4];	    //0x180
	unsigned long reserved3[28];
	unsigned long icdispr[4];	    //0x200
	unsigned long reserved4[28];
	unsigned long icdicpr[4];	    //0x280
	unsigned long reserved5[28];
	unsigned long icdiabr[4];	    //0x300
	unsigned long reserved6[60];
	unsigned long icdipr_sgi[4];	//0x400
	unsigned long icdipr_ppi[4];	//0x410
	unsigned long icdipr_spi[18];	//0x420
	unsigned long reserved7[230];
	unsigned long itargetsr[255];	//0x800
	unsigned long reserved9[1];
	unsigned long icdicfr[7];	    //0xc00
	unsigned long reserved8[185];
	unsigned long icdsgir;		    //0xf00
} gicd_reg, *p_gicd;


//CPU interface register map
typedef volatile struct gicc_reg {
	unsigned long iccicr;		//0x00
	unsigned long iccpmr;		//0x04
	unsigned long iccbpr;		//0x08
	unsigned long icciar;		//0x0c
	unsigned long icceoir;		//0x10
	unsigned long iccrpr;		//0x14
	unsigned long icchpir;		//0x18
	unsigned long iccabpr;		//0x1c
	unsigned long reserved0[55];
	unsigned long icciidr;		//0xfc
} gicc_reg, *p_gicc;


#define g_gicd       ((p_gicd)GICD)
#define g_gicc       ((p_gicc)GICC)


typedef void (*irq_handler)(void);

#define GIC_IRQS                         (5 * 32)

static irq_handler g_irq_handler[GIC_IRQS];

#define IRQ_GPIO7                        120  //P42

#define TIMER_6CH_0                      98  //前面那个6应该是总数的意思
#define TIMER_6CH_1                      99
#define TIMER_6CH_2                      100
#define TIMER_6CH_3                      101
#define TIMER_6CH_4                      102
#define TIMER_6CH_5                      103
#define TIMER_2CH_0                      104 //所以这里是2
#define TIMER_2CH_1                      105

extern void gic_irq_init(void);
extern void irq_handler_enable(int irq);
extern void register_irq(int irq, irq_handler handler);
extern void do_irq(void);
extern void gic_irq_set_trig(int irq, int ntrig);


#endif


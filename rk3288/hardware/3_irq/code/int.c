/*************
Function：interrupt
**************/ 

#include "int.h"
#include "uart.h"
#include "printf.h"

void gic_irq_init(void)
{   
	/* end of interrupt */
	g_gicc->icceoir = GIC_IRQS;
	/* disable signalling the interrupt */
	g_gicc->iccicr = 0x00;
	g_gicd->icddcr = 0x00;

	g_gicd->icdicer[0] = 0xFFFFFFFF;
	g_gicd->icdicer[1] = 0xFFFFFFFF;
	g_gicd->icdicer[2] = 0xFFFFFFFF;
	g_gicd->icdicer[3] = 0xFFFFFFFF;
	g_gicd->icdicfr[3] &= (~(1<<1));

	g_gicc->iccpmr  = 0xFF;    //int_set_prio_filt(0xff);
	g_gicc->iccicr |= 0x01;    //int_enable_secure_signal();
	g_gicc->iccicr |= 0x02;    //int_enable_nosecure_signal();
	g_gicd->icddcr  = 0x01;    //int_enable_distributor();

}

void gic_irq_set_trig(int irq, int ntrig)
{
	 unsigned long M, N;

	M = irq / 16;
	N = irq % 16;

	if (ntrig == 0)
		g_gicd->icdicfr[M] &= (~(1 << (2 * N + 1)));
	else
		g_gicd->icdicfr[M] |= (1 << (2 * N + 1));
}

void irq_handler_enable(int irq)
{
	unsigned long shift = (irq % 4) * 8;
	unsigned long offset = (irq / 4);
	unsigned long M, N;

    printf("irq_handler_enable: %d.\n\r", irq);

	M = irq / 32;
	N = irq % 32;
	g_gicc->iccicr &= (~0x08);
	g_gicd->icdiser[M] = (0x1 << N);
	g_gicd->itargetsr[offset] &= ~(0xFF << shift);
	g_gicd->itargetsr[offset] |= (1 << shift); //被这句坑了一天 = = 
}

/* register_irq*/
void register_irq(int irq, irq_handler handler)
{
    printf("register_irq: %d.\n\r", irq);

	if (g_irq_handler[irq] != handler)
		g_irq_handler[irq]= handler;
}


void do_irq(void)
{
	unsigned long get_int_id = g_gicc->icciar & 0x3FF; /* bit9 - bit0*/ /* get interrupt id */

    if (g_irq_handler[get_int_id])
        g_irq_handler[get_int_id]();

	g_gicc->icceoir = get_int_id; //gic_irq_finish_server
}	


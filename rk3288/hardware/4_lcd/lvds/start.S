	
#define CR_V	(1 << 13)	/* Vectors relocated to 0xffff0000	*/	
	
.text
.global  _start		
_start:	
	b reset                   @ _reset                 
	b halt                    @ _undefined_instruction 
	b halt                    @ _software_interrupt    
	b halt                    @ _prefetch_abort        
	b halt                    @ _data_abort            
	b halt                    @ _not_used              
	ldr	pc, _irq              @ _irq                   
	b halt                    @ _fiq                   

_irq:			
	.word irq

reset:		
        //msr     CPSR_c, #211
        msr	cpsr_c, #0xd3
#if 1
	/* disable interrupts (FIQ and IRQ), also set the cpu to SVC32 mode, except if in HYP mode already */
	mrs r0, cpsr
	and r1, r0, #0x1f		   @ mask mode bits
	teq r1, #0x1a		       @ test for HYP mode
	bicne	r0, r0, #0x1f      @ clear all mode bits
	orrne	r0, r0, #0x13      @ set SVC mode
	orr r0, r0, #0xc0		   @ disable FIQ and IRQ
	msr cpsr,r0
#endif	
	/* Setup vector: */
	/* Set V=0 in CP15 SCTRL register - for VBAR to point to vector */
	mrc	p15, 0, r0, c1, c0, 0	@ Read CP15 SCTRL Register
	bic	r0, #CR_V		@ V = 0
	mcr	p15, 0, r0, c1, c0, 0	@ Write CP15 SCTRL Register

	/* Set vector address in CP15 VBAR register */
	ldr	r0, =_start
	mcr	p15, 0, r0, c12, c0, 0	@Set VBAR

	bl enable_irq               @Enable FIQ and IRQ
	
	ldr  sp,=0x80000000	        @set stack

	bl relocate                 @relocate to ddr   
	
	bl main
halt1:
	b halt1        


enable_irq:
	/*
	mrs    r0, cpsr	     
    bic    r0, r0, #0x80      
    msr    cpsr_c, r0         
    mov    pc, lr             
	*/
	mrs r0, cpsr	
	bic	r0,r0,#0x9f             @ clear cpsr bit I, M4~M0, enable 
	orr	r0,r0,#0x13             
	msr	cpsr,r0                 @ enther SVC mode 
	mov    pc, lr   
	

	
relocate:
    adr r0, _start  
	ldr r1, =_start
				
	cmp r0, r1   
	moveq pc,lr  
	
	ldr r2, =__bss_start 
cpy:
	ldr r3, [r0], #4
	str r3, [r1], #4
	                 
    cmp r1, r2      
    bne cpy          
	
	mov pc, lr	
	
irq:
	/* 1. Save user regs */
	ldr sp, =(0x80000000 - 8*1024)
	sub lr, lr, #4
	stmdb sp!, {r0-r12, lr}  
	/* 2. Do irq */
	bl do_irq
	/* 3. Restore user regs */
	ldmia sp!, {r0-r12, pc}^ 
	
halt:
	b  halt 


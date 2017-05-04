
.extern     main
.text
.global _start
_start:

/* 0. 异常向量：本程序中，除Reset和HandleIRQ外，其它异常都没有使用*/
    b   Reset        			 	@ 0x00: 复位向量地址，上电运行
    
HandleUndef:
    b   HandleUndef 			 	@ 0x04: 未定义指令中止模式的向量地址
 
HandleSWI:          
    b   HandleSWI    			 	@ 0x08: 管理模式的向量地址，通过SWI指令进入此模式

HandlePrefetchAbort:
    b   HandlePrefetchAbort      	@ 0x0c: 指令预取终止导致的异常的向量地址

HandleDataAbort:
    b   HandleDataAbort   		 	@ 0x10: 数据访问终止导致的异常的向量地址

HandleNotUsed:
    b   HandleNotUsed  			 	@ 0x14: 保留

    b   HandleIRQ  				 	@ 0x18: 中断模式的向量地址

HandleFIQ:
    b   HandleFIQ  				 	@ 0x1c: 快中断模式的向量地址

Reset:  
/* 1. 关看门狗 ：向寄存器WTCON(0x53000000)写0即可；P462；*/
	ldr r0, =0x53000000   	     	@ 将0x53000000放入r0；
	mov r1, #0              	 	@ 将0放入r1中；
	str r1, [r0]            	 	@ 将r1中的0放入r0所指的存储器中；

/* 2. 设置中断模式栈指针，关闭中断使能*/
	msr cpsr_c, #0xd2      		 	@ 进入中断模式
    ldr sp, =3072         		 	@ 设置中断模式栈指针
    msr cpsr_c, #0xd3    		 	@ 返回管理模式

/* 3. 设置时钟：配置寄存器CLKDIVN(0x4C000014)设置分频；P260； */
	ldr r0, =0x4c000014
	mov r1, #0x05;			     	@ FCLK:HCLK:PCLK=1:4:8
	str r1, [r0]

/* 如果HDIVN非0，CPU的总线模式应设置为“asynchronous bus mode”;P244 */
	mrc	p15, 0, r1, c1, c0, 0	 	@ 读出控制寄存器 
	orr	r1, r1, #0xc0000000		 	@ 设置为“asynchronous bus mode” 
	mcr	p15, 0, r1, c1, c0, 0	 	@ 写入控制寄存器

/* 设置MPLLCON = S3C2440_MPLL_400MHZ; P256 */
	ldr r0, =0x4c000004
	ldr r1, =(0x5c<<12)|(0x01<<4)|(0x01)
	str r1, [r0]

/*优化速度： 启动ICACHE */
	mrc p15, 0, r0, c1, c0, 0	 	@ read control reg
	orr r0, r0, #(1<<12)
	mcr	p15, 0, r0, c1, c0, 0    	@ write it back


/* 4. 初始化SDRAM，依次向SDRAM寄存器 写入sdram_config中的值*/
	ldr r0, =0x48000000  		 	@ 将SDRAM寄存器基地址写入r0；
	adr r1, sdram_config   		 	@ 将sdram_config的当前地址写入r1;
	add r3, r0, #(13*4)    	 	 	@ r3=r0+13*4,即为SDRAM寄存器结尾地址；
1:
	ldr r2, [r1], #4       		 	@ 将r1寄存器的值放入r2，并将r1的地址+4;
	str r2, [r0], #4     		 	@ 将r2的值写入r0，并将r0的地址+4;
	cmp r0, r3 					 	@ 比较r0和r3，判断是否设置到了最后一个寄存器；
	bne 1b						 	@ 不相等即没有设置完，跳到前面(b)的1处；

/* 5. 重定位 : 把bootloader本身的代码从flash复制到它的链接地址去 */
	ldr sp, =0x34000000    		 	@ 在调用C语言前，需要先设置栈，即SP指向一个地址；
 
	bl nand_init          		 	@ 初始化nand flash；

	mov r0, #0              	 	@ 设置copy_code_to_sdram传入参数；参数1：r0=0；
	ldr r1, =_start         	 	@ 参数2：r1=_start；
	ldr r2, =__bss_start    	 	@ r2=__bss_start
	sub r2, r2, r1           	 	@ 参数3：r2=r2-r1 即除去_bss段的部分；
	
	bl copy_code_to_sdram   	 	@ 执行复制代码到SDRAM的函数；
	bl clear_bss            	 	@ 执行清理_bss段函数；
	 
/* 6. 初始化中断，开启中断使能*/
    //bl  init_led            		@ 初始化LED的GPIO管脚
    bl  init_irq            	 	@ 调用中断初始化函数，在init.c中
	msr cpsr_c, #0x53       	 	@ 设置I-bit=0，开IRQ中断

/* 7. 执行main */
	ldr lr, =halt
	ldr pc, =main
halt:
	b halt

sdram_config:            		 	@ 把要设置的值，存在这里；P56;
	.long 0x22011110	 		 	@ BWSCON
	.long 0x00000700	 		 	@ BANKCON0
	.long 0x00000700			 	@ BANKCON1
	.long 0x00000700			 	@ BANKCON2
	.long 0x00000700			 	@ BANKCON3  
	.long 0x00000700			 	@ BANKCON4
	.long 0x00000700			 	@ BANKCON5
	.long 0x00018005			 	@ BANKCON6
	.long 0x00018005			 	@ BANKCON7
	.long 0x008C04F4			 	@ REFRESH
	.long 0x000000B1			 	@ BANKSIZE
	.long 0x00000030			 	@ MRSRB6
	.long 0x00000030			 	@ MRSRB7

/*进入中断模式*/ 
HandleIRQ:
    sub lr, lr, #4               	@ 计算返回地址
    stmdb   sp!,    { r0-r12,lr }	@ 保存使用到的寄存器
                                 	@ 注意，此时的sp是中断模式的sp，初始值是上面设置的3072                           	
    ldr lr, =int_return          	@ 设置调用ISR即EINT_Handle函数后的返回地址  
    ldr pc, =IRQ_Handle             @ 调用中断服务函数，在interrupt.c中
int_return:
    ldmia   sp!,    { r0-r12,pc }^  @ 中断返回, ^表示将spsr的值复制到cpsr

    
@******************************************************************************
@ File：head.s
@ 功能：设置SDRAM，将程序复制到SDRAM，然后跳到SDRAM继续执行
@******************************************************************************       
 
.extern     main
.text
.global _start
_start:
/* 0. 异常向量，本程序中，除Reset和HandleIRQ外，其它异常都没有使用*/
    b   Reset

@ 0x04: 未定义指令中止模式的向量地址
HandleUndef:
    b   HandleUndef 
 
@ 0x08: 管理模式的向量地址，通过SWI指令进入此模式
HandleSWI:
    b   HandleSWI

@ 0x0c: 指令预取终止导致的异常的向量地址
HandlePrefetchAbort:
    b   HandlePrefetchAbort

@ 0x10: 数据访问终止导致的异常的向量地址
HandleDataAbort:
    b   HandleDataAbort

@ 0x14: 保留
HandleNotUsed:
    b   HandleNotUsed

@ 0x18: 中断模式的向量地址
    b   HandleIRQ

@ 0x1c: 快中断模式的向量地址
HandleFIQ:
    b   HandleFIQ

Reset:
    ldr sp, =4096           @设置堆栈 
    
    msr cpsr_c, #0xd2       @ 进入中断模式
    ldr sp, =3072           @ 设置中断模式栈指针

    msr cpsr_c, #0xd3       @ 进入管理模式
 /*   ldr sp, =4096         @ 设置管理模式栈指针， */
 
    bl  disable_watch_dog   @关WATCH DOG
    bl  clock_init
    
    
    bl  init_led            @ 初始化LED的GPIO管脚
    bl  init_irq            @ 调用中断初始化函数，在init.c中
	  msr cpsr_c, #0x53       @ 设置I-bit=0，开IRQ中断
	  
	  
    bl  memsetup            @初始化SDRAM   
    bl  nand_init           @初始化NAND Flash
                            @将NAND Flash中地址4096开始的1024字节代码(main.c编译得到)复制到SDRAM中
                            @nand_read_ll函数需要3个参数：
    ldr r0, =0x30000000     @1. 目标地址=0x30000000，这是SDRAM的起始地址
    mov r1, #4096           @2. 源地址   = 4096，连接的时候，main.c中的代码都存在NAND Flash地址4096开始处
    mov r2, #(1024*10)      @3. 复制长度= 1024(bytes)，对于本实验的main.c，这是足够了
    bl  nand_read           @调用C函数nand_read_ll

    ldr sp, =0x34000000     @设置堆栈
    ldr lr, =halt_loop      @设置返回地址
    ldr pc, =main           @b指令和bl指令只能前后跳转32M的范围，所以这里使用向pc赋值的方法进行跳转
halt_loop:
    b   halt_loop


HandleIRQ:
    sub lr, lr, #4                  @ 计算返回地址
    stmdb   sp!,    { r0-r12,lr }   @ 保存使用到的寄存器
                                    @ 注意，此时的sp是中断模式的sp
                                    @ 初始值是上面设置的3072
    
    ldr lr, =int_return             @ 设置调用ISR即EINT_Handle函数后的返回地址  
    ldr pc, =EINT_Handle            @ 调用中断服务函数，在interrupt.c中
int_return:
    ldmia   sp!,    { r0-r12,pc }^  @ 中断返回, ^表示将spsr的值复制到cpsr
    
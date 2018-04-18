	
.text
.global  _start
_start: 				

@设置栈
	ldr  sp,=0x80000000	@设置栈,2G DDR的最高位置

	bl relocate      @重定位
	@bl clean_bss     @清BSS段

	@adr r0, _start @可用于获取当前代码的地址,作为参数传给main,main里面再打印出来"int main(int addr)"
	@ldr  pc, =main @如果没重定位,这样直接跳到main代码的位置(链接脚本的期望地址),那个位置的数据未知,肯定出错
	bl main          @bl相对跳转,不管有没有重定位,都能到main的位置

halt:
	b  halt 
	
relocate:
    adr r0, _start  @r0:代码当前被放在的位置,实测出来是0x00000000,由硬件特性决定
	ldr r1, =_start @r1:代码期望被放在的位置,即链接脚本里的地址,用户想放在的位置,比如SDRAM
					@当两者相同则不用重定位,否则需要重定位
	cmp r0, r1      @比较r0和r1
	moveq pc,lr     @相等则pc=lr,即跳回到调用relocate的位置;不相等跳过执行下面的指令
	
	ldr r2, =__bss_start @r2等于链接脚本里的__bss_start,即代码段、只读数据段、数据段的结束位置
cpy:
	ldr r3, [r0], #4 @将r0地址的数据放到r3,r0往后再移动一个字节
	str r3, [r1], #4 @将r3的数据放到r1,r1往后再移动一个字节
	                 @这两句完成了代码从当前位置复制到期望的链接地址位置的操作
    cmp r1, r2       @判断是不是复制完了
    bne cpy          @不相等继续复制   
	
	mov pc, lr		     @pc=lr,即跳回到调用relocate的位置;
	
clean_bss:
	ldr r0, =__bss_start @r0=bss段开始位置
	ldr r1, =__bss_end   @r1=bss段结束位置
	mov r2, #0           @r0=0,填充0用
clean_loop:  
	str r2, [r0], #4     @将0写到bss段开始位置,并r0向后移一个字节
	cmp r0, r1           @比较bss段是不是完了
	bne clean_loop       @不相等则继续清0
	
	mov pc, lr		     @pc=lr,即跳回到调用clean_bss的位置;	
    
	



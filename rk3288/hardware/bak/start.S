	
.text
.global  _start
_start: 				

@…Ë÷√’ª
	ldr  sp,=0x80000000	@1024*2  2G

	mov r4, #0 
	bl relocate
	
	adr r0, _start
	bl main
	//ldr  pc, =main
	
relocate:
    adr r0, _start
	ldr r1, =_start
	cmp r0, r1
	moveq pc,lr
	
	ldr r2, =__bss_start
cpy:
	ldr r3, [r0]
    str r3, [r1]
    add r0, r0, #4
    add r1, r1, #4
    cmp r1, r2
    bne cpy
	mov r4, #1
    mov pc, lr		
	
	
halt:
	b  halt 



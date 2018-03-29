	
.text
.global  _start
_start: 				

@ÉèÖÃÕ»

	ldr  sp,=0x90000000
	
	bl  main

halt:
	b  halt 



SECTIONS {
    . = 0x70000000;  
    .text : { start.o(.text)
			  main.o(.text)
              led.o(.text)
			  uart.o(.text)
              *(.text) 
            }
    .rodata ALIGN(4) : {*(.rodata*)} 
    .data   ALIGN(4) : { *(.data) }
    __bss_start = .;
    .bss   ALIGN(4)  : { *(.bss)  *(COMMON) }
    __bss_end = .;
}

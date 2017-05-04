
CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall -O2
CPPFLAGS   	:= -nostdinc -nostdlib -I$(INCLUDEDIR)

export 	CC LD OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS

objs := head.o init.o nand.o serial.o interrupt.o main.o lib/libc.a

stdio.bin: $(objs)
	${LD} -Tstdio.lds -o stdio_elf $^
	${OBJCOPY} -O binary -S stdio_elf $@
	${OBJDUMP} -D -m arm stdio_elf > stdio.dis

.PHONY : lib/libc.a
lib/libc.a:
	cd lib; make; cd ..
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	make  clean -C lib
	rm -f stdio.bin stdio_elf stdio.dis *.o
	

objs := head.o init.o interrupt.o main.o

int.bin: $(objs)
	arm-linux-ld -Ttext 0x00000000 -o int_elf $^
	arm-linux-objcopy -O binary -S int_elf $@
	arm-linux-objdump -D -m arm int_elf > int.dis
	
%.o:%.c
	arm-linux-gcc -Wall -O2 -c -o $@ $<

%.o:%.S
	arm-linux-gcc -Wall -O2 -c -o $@ $<

clean:
	rm -f int.bin int_elf int.dis *.o		
	
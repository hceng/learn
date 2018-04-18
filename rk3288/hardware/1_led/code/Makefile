PREFIX=arm-none-linux-gnueabi-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

rk3288.bin : start.S  led.c main.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o led.o led.c	
	$(CC) -nostdlib -g -c -o main.o main.c	
	
	$(LD) -Trk3288.lds -g start.o led.o main.o -o rk3288_elf 
	
	$(OBJCOPY) -O binary -S rk3288_elf rk3288.bin
	$(OBJDUMP) -D -m arm  rk3288_elf > rk3288.dis	

clean:
	rm -f rk3288.dis  rk3288.bin rk3288_elf  *.o


	
PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

imx6ul.bin : start.S  led.c ddr.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o led.o led.c	
	$(CC) -nostdlib -g -c -o ddr.o ddr.c	
	$(LD) -Ttext 0x40300000 -g start.o led.o ddr.o -o imx6ul_elf
	$(OBJCOPY) -O binary -S imx6ul_elf imx6ul.bin
	$(OBJDUMP) -D -m arm  imx6ul_elf > imx6ul.dis	

clean:
	rm -f imx6ul.dis  imx6ul.bin imx6ul_elf  *.o


	
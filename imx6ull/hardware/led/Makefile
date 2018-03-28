PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

led.bin : start.S  led.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o led.o led.c	
	$(LD) -Ttext 0x40300000 -g start.o led.o -o led_elf
	$(OBJCOPY) -O binary -S led_elf led.bin
	$(OBJDUMP) -D -m arm  led_elf > led.dis	

clean:
	rm -f led.dis  led.bin led_elf  *.o


	
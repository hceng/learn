PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

# Add GCC lib
PLATFORM_LIBS += -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc

rk3288.bin : start.S  led.c uart.c  printf.c key.c timer.c int.c main.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o led.o led.c	
	$(CC) -nostdlib -g -c -o uart.o uart.c	
	$(CC) -nostdlib -g -c -o printf.o printf.c	
	$(CC) -nostdlib -g -c -o key.o key.c	
	$(CC) -nostdlib -g -c -o int.o int.c
	$(CC) -nostdlib -g -c -o timer.o timer.c		
	$(CC) -nostdlib -g -c -o main.o main.c	
	
	$(LD) -Trk3288.lds -g start.o led.o uart.o printf.o  key.o int.o timer.o main.o -o rk3288_elf  $(PLATFORM_LIBS)
	
	$(OBJCOPY) -O binary -S rk3288_elf rk3288.bin
	$(OBJDUMP) -D -m arm  rk3288_elf > rk3288.dis	

clean:
	rm -f rk3288.dis  rk3288.bin rk3288_elf  *.o


	

PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

#INCLUDEDIR 	:= $(shell pwd)/include/

#CPPFLAGS   	:= -nostdlib -I$(INCLUDEDIR) 

# Add GCC lib
PLATFORM_LIBS += -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc

imx6ul.bin : start.S  led.c uart.c main.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o led.o led.c	
	$(CC) -nostdlib -g -c -o uart.o uart.c	
	$(CC) -nostdlib -g -c -o main.o main.c	
	$(LD) -Timx6ul.lds -g start.o led.o uart.o main.o -o imx6ul_elf $(PLATFORM_LIBS)
	$(OBJCOPY) -O binary -S imx6ul_elf imx6ul.bin
	$(OBJDUMP) -D -m arm  imx6ul_elf > imx6ul.dis	

clean:
	rm -f imx6ul.dis  imx6ul.bin imx6ul_elf  *.o


	
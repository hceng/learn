KERN_DIR = /work/linux-4.13.9

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order app

obj-m	+= button_interrupt.o
obj-m	+= lcd_drv.o
obj-m	+= backlight_drv.o

app:
	arm-none-linux-gnueabi-gcc -o app app.c 

KERN_DIR = /work/linux-4.13.9

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order 

obj-m	+= regulator.o
obj-m	+= lcd_drv.o



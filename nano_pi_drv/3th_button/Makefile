KERN_DIR = /work/nano_pi/lichee/linux-3.4

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= button_drv.o

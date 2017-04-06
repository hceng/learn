KERN_DIR = /work/hceng/linux-3.4.2

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= led_drv.o
obj-m	+= led_dev.o


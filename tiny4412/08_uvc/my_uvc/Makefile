KERN_DIR = /usr/src/linux-headers-4.4.0-119-generic

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m += my_uvc.o




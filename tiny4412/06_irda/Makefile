KERN_DIR = /work/linux-4.13.9

rc-core-objs	:= rc-main.o rc-ir-raw.o

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m += rc-hceng-nec.o
obj-$(CONFIG_IR_GPIO_CIR) += gpio-ir-recv.o



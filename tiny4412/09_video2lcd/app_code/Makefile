# 1.定义编译工具简写并声明(以变其它文件可使用)
CROSS_COMPILE = arm-linux-gnueabihf-
AS            = $(CROSS_COMPILE)as
LD		      = $(CROSS_COMPILE)ld
CC		      = $(CROSS_COMPILE)gcc
CPP		      = $(CC) -E
AR		      = $(CROSS_COMPILE)ar
NM		      = $(CROSS_COMPILE)nm
              
STRIP		  = $(CROSS_COMPILE)strip
OBJCOPY		  = $(CROSS_COMPILE)objcopy
OBJDUMP		  = $(CROSS_COMPILE)objdump
export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

# 2.定义编译选项并声明(警告信息、优化等级、gdb调试、指定本程序头文件路径)
CFLAGS := -Wall -Werror -O2 -g
CFLAGS += -I $(shell pwd)/include
export CFLAGS

# 3.定义链接选项并声明(数学库、LibJPEG库)
LDFLAGS := -lm -ljpeg
export LDFLAGS

# 4.定义顶层目录路径并声明(shell命令实现)
TOPDIR := $(shell pwd)
export TOPDIR

# 5.程序目标文件
TARGET := video2lcd

# 6.使用"obj-y"表示各个目标文件,即过程中的所有.o文件(包含当前路径文件和当前路径下的文件夹)
obj-y += main.o
obj-y += video/
obj-y += convert/
obj-y += process/
obj-y += display/

# 7. 目标all:
# 7.1在-C指定目录下,执行指定路径下的文件(即在本路径执行Makefile.build)
# 7.2依赖"built-in.o"生成最终的目标文件
all : 
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC) -o $(TARGET) built-in.o $(LDFLAGS)

# 8.目标clean:清除所有的.o文件和目标文件
clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)

# 9.目标distclean:清除所有的.o文件、.d文件(依赖文件)和目标文件
distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	

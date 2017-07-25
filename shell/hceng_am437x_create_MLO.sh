#!/bin/bash

myPath="/mnt/hgfs/windows/"  #利用虚拟机"文件共享"WindowsU盘的路径
RED_COLOR='\E[1;31m'         #红
GREEN_COLOR='\E[1;32m'       #绿
YELOW_COLOR='\E[1;33m'       #黄
BLUE_COLOR='\E[1;34m'        #蓝
PINK='\E[1;35m'              #粉红
RES='\E[0m'                  #END

#生成MLO
function creat_MLO()
{
	rm -f /mnt/hgfs/windows/MLO
	make clean 
	make
	gcc  ./image_tool/tiimage.c  -o  tiimage.out
	./tiimage.out   0x40300000    MMCSD    am437x_hardware.bin   /mnt/hgfs/windows/MLO  
	rm   -f *.dis  *.bin  *.o  *_elf tiimage.out
	ls /mnt/hgfs/windows/MLO -l && echo -e  "${GREEN_COLOR}======MLO OK!======${RES}" || echo -e  "${RED_COLOR}======MLO ERROR!======${RES}"
	sync
}

#判断U盘路径是否存在；
#存在则生成MLO、拷贝，不存在则提示报错
if [ ! -d "$myPath" ];then  
  echo -e  "${RED_COLOR}======Check USB Disk!======${RES}"	
else
	creat_MLO	 
fi  
sync
exit

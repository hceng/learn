#!/bin/bash

#1.check network
#2.update software source
#3.install vim screenfetch tmux  htop ftp、ssh、nfs、samba
 #3.1 configure vim
 #3.2 configure ftp
 #3.3 configure nfs
 #3.4 configure samba
#4.install system tool eg:g++ ...

#define echo print color.
RED_COLOR='\E[1;31m===='        
PINK_COLOR='\E[1;35m===='       
YELOW_COLOR='\E[1;33m===='       
BLUE_COLOR='\E[1;34m===='       
GREEN_COLOR='\E[1;32m===='      
END_COLOR='====\E[0m'           

check_status() {
    ret=$?
    if [ "$ret" -ne "0" ]; then
        echo -e "${RED_COLOR}Failed setup, aborting..${END_COLOR}" 
        exit 1
    fi
}

# This function will return the code name of the Linux host release to the caller.
get_host_type() {
    local  __host_type=$1
    local  the_host=`lsb_release -a 2>/dev/null | grep Codename: | awk {'print $2'}`
    eval $__host_type="'$the_host'"
}

vim_configure() {
	local back_file=$HOME/.vimrc.bakup
	if [ ! -e "$back_file" ];then  
		cp $HOME/.vimrc $back_file
	fi
	
	local back_file=$HOME/.vim.babkup
	if [ ! -d "$back_file" ];then  
		cp -r $HOME/.vim $back_file
	fi
	rm -r $HOME/.vim
	
	wget -O - https://raw.githubusercontent.com/vgod/vimrc/master/auto-install.sh | sh  && cp $HOME/.vim/vimrc ../.vimrc
	chmod -R 777 $HOME/.vim
}

ftp_configure() {
	sed  -i 's/#loacl_enable=YES/loacl_enable=YES/g'  /etc/vsftpd.conf
	sed  -i 's/#write_enable=YES/write_enable=YES/g'  /etc/vsftpd.conf
}

nfs_configure() {
	local work_file=/work
	if [ ! -e "$work_file" ];then  
		mkdir /work
	fi
	
	grep "/work" /etc/exports 1>/dev/null
	if [ $? -ne 0];then
		sed -i '$a\/work  *(rw,sync,no_root_squash,no_subtree_check)' /etc/exports
	fi
}

samba_configure() {
	local back_file=/etc/samba/smb.conf.bakup
	if [ ! -e "$back_file" ];then  
		cp /etc/samba/smb.conf $back_file		 
	fi 
	check_status
	
	grep "/work" /etc/samba/smb.conf 1>/dev/null
	if [ $? -ne 0 ];then
		sed -i '$a[share_work]\n\
		path = \/work\n\
		available = yes\n\
		public = yes\n\
		guest ok = yes\n\
		read only = no\n\
		writeable = yes\n' /etc/samba/smb.conf
	fi
	
	#touch /etc/samba/smbpasswd
	#echo -e "${PINK_COLOR}Please set the samba password.${END_COLOR}"
	#smbpasswd -a hceng

	/etc/init.d/samba restart
	chmod -R 777 /work
}

# Execute an action
FA_DoExec() {
	echo "==> Executing: '${@}'"
	eval $@ || exit $?
}

check_network() {
	ping -c 1 www.hceng.cn > /dev/null 2>&1  
	if [ $? -eq 0 ];then  
		echo -e "${GREEN_COLOR}Network ok.${END_COLOR}" 
	else
		echo -e "${RED_COLOR}Network failure!${END_COLOR}"  
	fi
}

#This function will update the source of the software.
update_software_source() {
	local back_file=/etc/apt/sources.list.backup
	if [ ! -e "$back_file" ];then  
		cp /etc/apt/sources.list $back_file		 
	fi 
	check_status
	
	get_host_type host_release
	
	echo \
"#Ali source.
deb-src http://archive.ubuntu.com/ubuntu $host_release main restricted #Added by software-properties
deb http://mirrors.aliyun.com/ubuntu/ $host_release main restricted
deb-src http://mirrors.aliyun.com/ubuntu/ $host_release main restricted multiverse universe #Added by software-properties
deb http://mirrors.aliyun.com/ubuntu/ $host_release-updates main restricted
deb-src http://mirrors.aliyun.com/ubuntu/ $host_release-updates main restricted multiverse universe #Added by software-properties
deb http://mirrors.aliyun.com/ubuntu/ $host_release universe
deb http://mirrors.aliyun.com/ubuntu/ $host_release-updates universe
deb http://mirrors.aliyun.com/ubuntu/ $host_release multiverse
deb http://mirrors.aliyun.com/ubuntu/ $host_release-updates multiverse
deb http://mirrors.aliyun.com/ubuntu/ $host_release-backports main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ $host_release-backports main restricted universe multiverse #Added by software-properties
deb http://archive.canonical.com/ubuntu $host_release partner
deb-src http://archive.canonical.com/ubuntu $host_release partner
deb http://mirrors.aliyun.com/ubuntu/ $host_release-security main restricted
deb-src http://mirrors.aliyun.com/ubuntu/ $host_release-security main restricted multiverse universe #Added by software-properties
deb http://mirrors.aliyun.com/ubuntu/ $host_release-security universe
deb http://mirrors.aliyun.com/ubuntu/ $host_release-security multiverse

#Netease source.
deb http://mirrors.163.com/ubuntu/ $host_release main restricted universe multiverse
deb http://mirrors.163.com/ubuntu/ $host_release-security main restricted universe multiverse
deb http://mirrors.163.com/ubuntu/ $host_release-updates main restricted universe multiverse
deb http://mirrors.163.com/ubuntu/ $host_release-proposed main restricted universe multiverse
deb http://mirrors.163.com/ubuntu/ $host_release-backports main restricted universe multiverse
deb-src http://mirrors.163.com/ubuntu/ $host_release main restricted universe multiverse
deb-src http://mirrors.163.com/ubuntu/ $host_release-security main restricted universe multiverse
deb-src http://mirrors.163.com/ubuntu/ $host_release-updates main restricted universe multiverse
deb-src http://mirrors.163.com/ubuntu/ $host_release-proposed main restricted universe multiverse
deb-src http://mirrors.163.com/ubuntu/ $host_release-backports main restricted universe multiverse

#Official source
deb http://archive.ubuntu.com/ubuntu/ $host_release main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ $host_release-security main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ $host_release-updates main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ $host_release-proposed main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ $host_release-backports main restricted universe multiverse
deb-src http://archive.ubuntu.com/ubuntu/ $host_release main restricted universe multiverse
deb-src http://archive.ubuntu.com/ubuntu/ $host_release-security main restricted universe multiverse
deb-src http://archive.ubuntu.com/ubuntu/ $host_release-updates main restricted universe multiverse
deb-src http://archive.ubuntu.com/ubuntu/ $host_release-proposed main restricted universe multiverse
deb-src http://archive.ubuntu.com/ubuntu/ $host_release-backports main restricted universe multiverse" > /etc/apt/sources.list
	check_status
	#apt-get update 1>/dev/null
	apt-get update 
	
	echo -e "${GREEN_COLOR}Update source completed.${END_COLOR}" 
}

install_software() {
	local install_software_list=("git" "vim" "tmux" "htop" "vsftpd" "openssh-server" "nfs-kernel-server" "portmap" "samba")
	echo -e "${BLUE_COLOR}install_software_list:${install_software_list[*]}.${END_COLOR}" 
	
	#install git
	if (echo "${install_software_list[@]}" | grep -wq "git");then
		apt-get -y install git && echo -e "${BLUE_COLOR}git install completed.${END_COLOR}" 
	fi
	
	#install and configure vim
	if (echo "${install_software_list[@]}" | grep -wq "vim");then
		apt-get -y install vim && vim_configure	&& echo -e "${BLUE_COLOR}vim install completed.${END_COLOR}" 
	fi
	
	#install tmux
	if (echo "${install_software_list[@]}" | grep -wq "tmux");then
		apt-get -y install tmux && echo -e "${BLUE_COLOR}tmux install completed.${END_COLOR}" 
	fi
	
	#install htop
	if (echo "${install_software_list[@]}" | grep -wq "htop");then
		apt-get -y install htop && echo -e "${BLUE_COLOR}htop install completed.${END_COLOR}" 
	fi
	
	#install and configure vsftpd
	if (echo "${install_software_list[@]}" | grep -wq "vsftpd");then
		apt-get -y install vsftpd && ftp_configure && echo -e "${BLUE_COLOR}vsftpd install completed.${END_COLOR}" 
	fi
	
	#install openssh-server
	if (echo "${install_software_list[@]}" | grep -wq "openssh-server");then
		apt-get -y install openssh-server && echo -e "${BLUE_COLOR}openssh-server install completed.${END_COLOR}" 
	fi
	
	#install and configure nfs-kernel-server
	if (echo "${install_software_list[@]}" | grep -wq "nfs-kernel-server");then
		apt-get -y install nfs-kernel-server && nfs_configure && /etc/init.d/nfs-kernel-server restart && echo -e "${BLUE_COLOR}nfs-kernel-server install completed.${END_COLOR}" 
	fi
	
	#install portmap
	if (echo "${install_software_list[@]}" | grep -wq "portmap");then
		apt-get -y install portmap && echo -e "${BLUE_COLOR}portmap install completed.${END_COLOR}" 
	fi
	
	#install and configure samba
	if (echo "${install_software_list[@]}" | grep -wq "samba");then
		apt-get -y install samba && samba_configure && echo -e "${BLUE_COLOR}samba install completed.${END_COLOR}" 
	fi
	
	
	#others
	get_host_type host_release
	
	FA_DoExec apt-get -y install \
	gnupg flex bison gperf build-essential \
	zip curl libc6-dev libncurses5-dev libncurses5-dev:i386 x11proto-core-dev \
	libx11-dev:i386 libreadline6-dev:i386 \
	libgl1-mesa-glx-lts-$host_release:i386 libgl1-mesa-dev-lts-$host_release \
	g++-multilib mingw32 tofrodos libncurses5-dev:i386 \
	python-markdown libxml2-utils xsltproc zlib1g-dev:i386


	if [ ! -h /usr/lib/i386-linux-gnu/libGL.so ]; then
	FA_DoExec ln -s /usr/lib/i386-linux-gnu/mesa/libGL.so.1 \
		/usr/lib/i386-linux-gnu/libGL.so
	fi

	# Development support
	FA_DoExec apt-get -y install \
	dos2unix minicom gawk

	echo -e "${GREEN_COLOR}software install completed.${END_COLOR}"
}

# Check if user is root
if [ $(id -u) != "0" ]; then
    echo "${RED_COLOR}Error: You must be root to run this script, please use root.${END_COLOR}"
    exit 1
fi

check_network
check_status
update_software_source
check_status
install_software
check_status


echo -e "${GREEN_COLOR}===================================================${END_COLOR}" 
echo -e "${GREEN_COLOR}============setup ubuntu host env ok!==============${END_COLOR}" 
echo -e "${GREEN_COLOR}===================================================${END_COLOR}"
 
exit 0









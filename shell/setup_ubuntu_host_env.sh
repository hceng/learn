#!/bin/bash

# -------------------------------------------------------------------------------
# Filename:    setup_ubuntu_host_env.sh
# Revision:    1.0
# Date:        2017/08/05
# Author:      hceng
# Email:       huangcheng.job@foxmail.com
# Website:     www.hceng.cn
# Function:    setup ubuntu host env.
# Notes:       learn
# -------------------------------------------------------------------------------
#
# Description:     
#1.check env.
#1.1 check network  
#1.2 check use root  
#1.3 check set name  
#1.4 configure samba   
#2.update software sourcev.  
#3.install vim tmux  htop ftp ssh nfs samba.
#3.1 configure vim  
#3.2 configure ftp  
#3.3 configure nfs  
#3.4 configure samba 
#4.install system tool eg:g++ ...
#
# -------------------------------------------------------------------------------


#define echo print color.
RED_COLOR='\E[1;31m'        
PINK_COLOR='\E[1;35m'       
YELOW_COLOR='\E[1;33m'       
BLUE_COLOR='\E[1;34m'       
GREEN_COLOR='\E[1;32m'      
END_COLOR='\E[0m'           

#Set linux host user name.
user_name=hceng

# Check network.
check_network() {
	ping -c 1 www.baidu.com > /dev/null 2>&1  
	if [ $? -eq 0 ];then  
		echo -e "${GREEN_COLOR}Network OK.${END_COLOR}" 
	else
		echo -e "${RED_COLOR}Network failure!${END_COLOR}"  
		exit 1
	fi
}

# Check user must root.
check_root() {
	if [ $(id -u) != "0" ]; then
		echo -e "${RED_COLOR}Error: You must be root to run this script, please use root.${END_COLOR}"
		exit 1
	fi
}

# Check set linux host user name.
check_user_name() {
	cat /etc/passwd|grep $user_name
	if [ $? -eq 0 ];then
		echo -e "${GREEN_COLOR}Check the set user name OK.${END_COLOR}" 
	else
		echo -e "${RED_COLOR}Check the set user name failure!${END_COLOR}" 
		exit 1
	fi
}

# Check the results of the operation.
check_status() {
    ret=$?
    if [ "$ret" -ne "0" ]; then
        echo -e "${RED_COLOR}Failed setup, aborting..${END_COLOR}" 
        exit 1
    fi
}

# Get the code name of the Linux host release to the caller.
get_host_type() {
    local  __host_type=$1
    local  the_host=`lsb_release -a 2>/dev/null | grep Codename: | awk {'print $2'}`
    eval $__host_type="'$the_host'"
}


#This function will update the source of the software.
update_software_source() {
	local back_file=/etc/apt/sources.list.backup
	if [ ! -e "$back_file" ];then  
		cp /etc/apt/sources.list $back_file		 
	fi 
	check_status
	
	get_host_type host_release
	check_status
	
	echo \
	"#Ali source.
	deb-src http://archive.ubuntu.com/ubuntu    $host_release main restricted 
	deb http://mirrors.aliyun.com/ubuntu/       $host_release main restricted
	deb-src http://mirrors.aliyun.com/ubuntu/   $host_release main restricted multiverse universe 
	deb http://mirrors.aliyun.com/ubuntu/       $host_release-updates main restricted
	deb-src http://mirrors.aliyun.com/ubuntu/   $host_release-updates main restricted multiverse universe
	deb http://mirrors.aliyun.com/ubuntu/       $host_release universe
	deb http://mirrors.aliyun.com/ubuntu/       $host_release-updates universe
	deb http://mirrors.aliyun.com/ubuntu/       $host_release multiverse
	deb http://mirrors.aliyun.com/ubuntu/       $host_release-updates multiverse
	deb http://mirrors.aliyun.com/ubuntu/       $host_release-backports main restricted universe multiverse
	deb-src http://mirrors.aliyun.com/ubuntu/   $host_release-backports main restricted universe multiverse 
	deb http://archive.canonical.com/ubuntu     $host_release partner
	deb-src http://archive.canonical.com/ubuntu $host_release partner
	deb http://mirrors.aliyun.com/ubuntu/       $host_release-security main restricted
	deb-src http://mirrors.aliyun.com/ubuntu/   $host_release-security main restricted multiverse universe 
	deb http://mirrors.aliyun.com/ubuntu/       $host_release-security universe
	deb http://mirrors.aliyun.com/ubuntu/       $host_release-security multiverse

	#Netease source.
	deb http://mirrors.163.com/ubuntu/          $host_release main restricted universe multiverse
	deb http://mirrors.163.com/ubuntu/          $host_release-security main restricted universe multiverse
	deb http://mirrors.163.com/ubuntu/          $host_release-updates main restricted universe multiverse
	deb http://mirrors.163.com/ubuntu/          $host_release-proposed main restricted universe multiverse
	deb http://mirrors.163.com/ubuntu/          $host_release-backports main restricted universe multiverse
	deb-src http://mirrors.163.com/ubuntu/      $host_release main restricted universe multiverse
	deb-src http://mirrors.163.com/ubuntu/      $host_release-security main restricted universe multiverse
	deb-src http://mirrors.163.com/ubuntu/      $host_release-updates main restricted universe multiverse
	deb-src http://mirrors.163.com/ubuntu/      $host_release-proposed main restricted universe multiverse
	deb-src http://mirrors.163.com/ubuntu/      $host_release-backports main restricted universe multiverse
                                                
	#Official source                            
	deb http://archive.ubuntu.com/ubuntu/       $host_release main restricted universe multiverse
	deb http://archive.ubuntu.com/ubuntu/       $host_release-security main restricted universe multiverse
	deb http://archive.ubuntu.com/ubuntu/       $host_release-updates main restricted universe multiverse
	deb http://archive.ubuntu.com/ubuntu/       $host_release-proposed main restricted universe multiverse
	deb http://archive.ubuntu.com/ubuntu/       $host_release-backports main restricted universe multiverse
	deb-src http://archive.ubuntu.com/ubuntu/   $host_release main restricted universe multiverse
	deb-src http://archive.ubuntu.com/ubuntu/   $host_release-security main restricted universe multiverse
	deb-src http://archive.ubuntu.com/ubuntu/   $host_release-updates main restricted universe multiverse
	deb-src http://archive.ubuntu.com/ubuntu/   $host_release-proposed main restricted universe multiverse
	deb-src http://archive.ubuntu.com/ubuntu/   $host_release-backports main restricted universe multiverse" \
	> /etc/apt/sources.list
	check_status
	
	#apt-get update 1>/dev/null
	apt-get update
	check_status	
	
	echo -e "${GREEN_COLOR}Update source completed.${END_COLOR}" 
}


# Configure vim form github.
vim_configure() {
	git clone --depth=1 https://github.com/amix/vimrc.git /home/$user_name/.vim_runtime 
	
	touch /home/$user_name/.vim_runtime/my_configs.vim
	echo ":set number" > /home/$user_name/.vim_runtime/my_configs.vim
	
	chown -R $user_name /home/$user_name/.vim_runtime 
	chmod u+x /home/$user_name/.vim_runtime/install_awesome_vimrc.sh
	su - $user_name -s /home/$user_name/.vim_runtime/install_awesome_vimrc.sh
}

# Configure ftp.
ftp_configure() {
	sed  -i 's/#loacl_enable=YES/loacl_enable=YES/g'  /etc/vsftpd.conf
	sed  -i 's/#write_enable=YES/write_enable=YES/g'  /etc/vsftpd.conf
}

# Configure nfs.
nfs_configure() {
	local work_file=/work
	if [ ! -d "$work_file" ];then  
		mkdir /work
	fi
	
	grep "/work" /etc/exports 1>/dev/null
	if [ $? -ne 0 ];then
		sed -i '$a\/work  *(rw,sync,no_root_squash,no_subtree_check)' /etc/exports
	fi
}

# Configure samba.
samba_configure() {
	local back_file=/etc/samba/smb.conf.bakup
	if [ ! -e "$back_file" ];then  
		cp /etc/samba/smb.conf $back_file		 
	fi 
	check_status
	
	grep "/work" /etc/samba/smb.conf 1>/dev/null
	if [ $? -ne 0 ];then
		sed -i \
		'$a[share_work]\n\
		path = \/work\n\
		available = yes\n\
		public = yes\n\
		guest ok = yes\n\
		read only = no\n\
		writeable = yes\n' /etc/samba/smb.conf
	fi
	
	/etc/init.d/samba restart
	chmod -R 777 /work
}

# Execute an action.
FA_DoExec() {
	echo -e "${BLUE_COLOR}==> Executing: '${@}'.${END_COLOR}"
	eval $@ || exit $?
}

# Install list software.
install_software() {
	local install_software_list=("git" "vim" "tmux" "htop" "vsftpd" "openssh-server" "nfs-kernel-server" "portmap" "samba")
	echo -e "${PINK_COLOR}install_software_list:${install_software_list[*]}.${END_COLOR}" 
	
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
		apt-get -y install nfs-kernel-server && nfs_configure && \
		/etc/init.d/nfs-kernel-server restart && echo -e "${BLUE_COLOR}nfs-kernel-server install completed.${END_COLOR}" 
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

check_network
check_root
check_user_name

update_software_source
install_software

echo -e "${GREEN_COLOR}===================================================${END_COLOR}" 
echo -e "${GREEN_COLOR}============setup ubuntu host env ok!==============${END_COLOR}" 
echo -e "${GREEN_COLOR}===================================================${END_COLOR}"

su $user_name
 
exit 0

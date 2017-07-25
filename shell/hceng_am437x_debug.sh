#!/bin/sh

#add this shell script to /etc/profile

#hceng add for drv_debug.
tail -f /var/log/messages &

#hceng add for close GUI.
program=`ps | grep -v grep | grep matrix_browser`  
if ["$program" = ""]  
then  
    exit  
else  
    /etc/init.d/matrix-gui-2.0 stop
fi 

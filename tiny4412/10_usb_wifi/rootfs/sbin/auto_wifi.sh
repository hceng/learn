#!/bin/sh
if [ $ACTION = "add" ];
then                                
   echo --------usb wifi connect--------- > /dev/console
   wpa_supplicant -B -d -i wlan0 -c /etc/wpa_supplicant.conf  
   wpa_cli -B -i wlan0 -a/sbin/auto_dhcp.sh 
else                     
   echo --------usb wifi disconnect--------- > /dev/console
   killall wpa_supplicant
   killall wpa_cli 
   killall dhclient
fi

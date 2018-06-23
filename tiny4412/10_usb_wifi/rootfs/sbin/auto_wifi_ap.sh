#!/bin/sh
if [ $ACTION = "add" ];
then                                
   echo --------usb wifi connect--------- > /dev/console
   hostapd -B /etc/hostapd.conf
   ifconfig wlan0 192.168.2.1
   dhcpd -cf /etc/dhcpcd.conf wlan0

else                     
   echo --------usb wifi disconnect--------- > /dev/console
   killall hostapd
   killall dhcpd
fi

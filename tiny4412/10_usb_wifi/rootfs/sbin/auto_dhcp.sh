#!/bin/sh
IFNAME=$1
CMD=$2
if [ "$CMD" = "CONNECTED" ]; then
   echo ========connect $IFNAME, dhclient for it======= > /dev/console
   dhclient $IFNAME
fi
if [ "$CMD" = "DISCONNECTED" ]; then
   echo ========disconnect $IFNAME, kill dhclient for it======= > /dev/console
   killall dhclient
fi

#!/bin/sh
#chkconfig: 2345 80 90
#description:auto_run

nohup /usr/local/python3/bin/python3 -u /root/SSPort.py > /root/SSPort.log 2>&1 &


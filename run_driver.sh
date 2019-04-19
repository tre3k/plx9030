#!/bin/bash

drv_start(){
    modprobe plx9030
    chown root:plxdev /dev/plxdev*
}

drv_stop(){
    rmmod plx9030
}

case $1 in
    start)
	drv_start
	;;
    stop)
	drv_stop
	;;
esac

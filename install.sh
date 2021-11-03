#!/bin/bash

make

mkdir -p /lib/modules/`uname -r`/extra/
cp plx9030.ko /lib/modules/`uname -r`/extra/
depmod -a
modprobe plx9030

chmod 666 /dev/plxdev*

echo "plx9030" >> /etc/modules
echo "KERNEL==\"plxdev[0-9]*\", MODE=\"0666\"" > /etc/udev/rules.d/10-plxdetector.rules

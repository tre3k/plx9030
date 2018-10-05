#!/bin/bash

make

mkdir -p /lib/modules/`uname -r`/extra/
cp plx9030.ko /lib/modules/`uname -r`/extra/
depmod -a
modprobe plx9030

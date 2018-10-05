obj-m := plx9030.o
KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)
GCC:=gcc

all: default

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
test:
	rmmod plx9030
	insmod plx9030.ko
	dmesg | tail -n 40

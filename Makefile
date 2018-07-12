obj-m := plx9030.o
KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

all: default

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

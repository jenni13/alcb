ifneq ($(KERNELRELEASE),)
	obj-m := alcb.o
#	Hello_World_4-objs := Hello_World_4_Start.o Hello_World_4_Stop.o
else
	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C ${KERNEL_DIR} M=$(PWD) modules
endif

TARGET_MODULE = alcb

ifneq ($(KERNELRELEASE),)
	obj-m := alcb.o
#	Hello_World_4-objs := Hello_World_4_Start.o Hello_World_4_Stop.o
else
	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C ${KERNEL_DIR} M=$(PWD) modules

clean:
# run kernel build system to cleanup in current directory
	rm $(TARGET_MODULE).ko
	rm $(TARGET_MODULE).mod.c
	rm *.o
	rm modules.order
	rm -f Module.symvers
endif

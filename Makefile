PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
DRV_NAME := rw_kernel
CC := x86_64-linux-gnu-gcc

obj-m += $(DRV_NAME).o

.PHONY: build run remove install uninstall clean check format all

all: build app_us

app_us:
	gcc -o app_us app_us.c -lpthread

build:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

run:
	insmod $(PWD)/$(DRV_NAME).ko

remove:
	rmmod $(DRV_NAME).ko

install:
	cp $(DRV_NAME).ko /lib/modules/$(shell uname -r)
	/sbin/depmod -a
	/sbin/modprobe $(DRV_NAME)

uninstall:
	/sbin/modprobe -r $(DRV_NAME)
	rm -f /lib/modules/$(shell uname -r)/$(DRV_NAME).ko
	/sbin/depmod -a

clean: 
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf app_us *.o *.mod.* *.order *.symvers

format:
	clang-format -i *.c *.h Makefile

check:
	cppcheck --enable=all *.c

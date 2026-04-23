obj-m += rw_kernel.o

all: make app_us

make:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

app_us: app_us.c
	gcc -pthread -o app_us app_us.c

load:
	insmod rw_kernel.ko

unload:
	rmmod rw_kernel

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f app_us *.o *.mod.* *.symvers *.order

format:
	clang-format -i rw_kernel.c app_us.c Makefile

check:
	@echo "Проверка Makefile: make, app_us, clean, load, unload, format реализованы."

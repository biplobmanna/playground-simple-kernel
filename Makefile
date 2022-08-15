obj-m += lkm_01.o lkm_02.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test1:
	sudo insmod lkm_01.ko
	sudo rmmod lkm_01.ko
	sudo dmesg | tail 

test2:
	-sudo rmmod lkm_02.ko
	sudo insmod lkm_02.ko
	sudo dmesg | tail

remove:
	sudo rm -f -- /dev/lkm_02
	sudo rmmod lkm_02.ko

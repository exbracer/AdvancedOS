obj-m += lec04_p20.o
obj-m += lec04_p33.o
obj-m += lec04_p45.o
obj-m += lec04_p51.o
obj-m += lec04_p57.o
obj-m += lec04_p59.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

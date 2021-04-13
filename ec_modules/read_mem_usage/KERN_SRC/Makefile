CONFIG_MODULE_SIG=n
obj-m		:= read_mem_usage.o 
KERN_SRC	:= /lib/modules/$(shell uname -r)/build/
PWD			:= $(shell pwd)

modules:
	make -C $(KERN_SRC) M=$(PWD) modules

install:
	make -C $(KERN_SRC) M=$(PWD) modules_install
	depmod -a

clean:
	make -C $(KERN_SRC) M=$(PWD) clean
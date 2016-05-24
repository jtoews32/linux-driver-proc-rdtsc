KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m := cpuspeed.o

all: clean run 
	@make -s clean

run: load
	dd if=/proc/cpuspeed count=1;

load: cpuspeed.o
	-su -c "insmod ./cpuspeed.ko" 

cpuspeed.o:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

unload:
	-su -c "rmmod cpuspeed"

clean: unload
	-@rm -fr *.o cpuspeed*.o cpuspeed*.ko .cpuspeed*.* cpuspeed*.*.* .tmp_versions [mM]odule*


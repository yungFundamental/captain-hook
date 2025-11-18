# Name of module
obj-m := captain.o

# Objects that form the module (main + trampoline)
captain-objs := trampoline/trampoline.o main.o

ccflags-y += -I$(src)

# Kernel build system
KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

# Add include path for trampoline.h
ccflags-y += -I$(PWD)/trampoline

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean


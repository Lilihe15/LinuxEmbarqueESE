obj-m:=gpio-leds.o
KERNEL_SOURCE=/home/ensea/linux-socfpga
CFLAGS_MODULE= -fno-pic

all:
        make -C $(KERNEL_SOURCE) M=$(PWD) modules
clean:
        make -C $(KERNEL_SOURCE) M=$(PWD) clean
install:
        make -C $(KERNEL_SOURCE) M=$(PWD) modules install

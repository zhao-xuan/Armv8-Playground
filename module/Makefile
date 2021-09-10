# obj-m += virtual_address.o
obj-m += memory_attribute.o
# obj-m += mmu_translation_walk.o
# obj-m += module_deref_null.o
# obj-m += ecount.o
# obj-m += hello-1.o
# obj-m += hello-5.o

# LINUX_DIR=/home/xuazha01/Desktop/buildroot-2021.05/output/build/linux-5.10.7/
# LINUX_DIR=/home/xuazha01/Desktop/buildroot-2021.05/output/images/squashfs-root
LINUX_DIR=/home/xuazha01/Desktop/linux

all:
	make -C $(LINUX_DIR) M=$(PWD) modules

clean:
	make -C $(LINUX_DIR) M=$(PWD) clean

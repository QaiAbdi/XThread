# Find CPU info in shel by 'cat /proc/cpuinfo'.
#FPU enabled.
# -mhard-floatis to generate code containing hardware floating-point instructions (FPU).
#EXTRA_CFLAGS := -O3 -mfpmath=387 -mhard-float

EXTRA_CFLAGS := -O3

# SSE2 enabled
#EXTRA_CFLAGS := -O3 -mfpmath=sse -msse2


obj-m += Mymodule.o
Mymodule-objs := kthreadQ.o kthread_extended.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	${shell cat /dev/null > /var/log/messages}
	#echo rmmod
	$(shell rmmod Mymodule.ko -f)
	#echo insmod
	$(shell insmod ./Mymodule.ko)

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(shell rm *.ko *.o -f)
obj-m += main.o


all:
	make -C /home/xpumapu/ath M=$(PWD) modules

clean:
	make -C /home/xpumapu/ath M=$(PWD) clean


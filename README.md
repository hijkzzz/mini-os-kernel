##自制操作系统
从零开始，自制一个简单的内核

##编译
安装依赖环境
```
sudo apt-get install build-essential nasm cgdb qemu
sudo ln -s /usr/bin/qemu-system-i386 /usr/bin/qemu
sudo mkdir /mnt/floppy
```
运行
```
make
make qemu
```

##完成进度
~~启动~~
~~显示驱动~~
~~中断~~
~~键盘驱动~~
~~物理内存管理~~
~~虚拟内存管理~~
~~进程管理~~
~~任务调度~~
系统调用
TTY
进程间通信
文件系统
安装应用程序

##参考资料
JamesM's kernel development tutorials
一个操作系统的实现
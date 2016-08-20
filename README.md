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
* ~~启动~~
* ~~字符打印~~
* ~~中断服务~~
* ~~键盘输入~~
* ~~物理内存管理~~
* ~~虚拟内存管理~~
* ~~内核进程~~
* ~~任务调度~~
* 用户进程
* 系统调用
* 终端
* 进程间通信
* 文件系统
* 运行应用程序

##参考资料
* JamesM's kernel development tutorials
* 一个操作系统的实现

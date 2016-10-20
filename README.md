## 自制操作系统
自制一个简单的类 UNIX 内核

## 编译&运行
```
// 配置依赖环境
sudo apt-get install build-essential nasm cgdb qemu
sudo ln -s /usr/bin/qemu-system-i386 /usr/bin/qemu
sudo mkdir /mnt/floppy

make
make qemu
```

## TODO
* ~~启动~~
* ~~字符打印~~
* ~~中断服务~~
* ~~键盘输入~~
* ~~物理内存管理~~
* ~~虚拟内存管理~~
* ~~内核进程~~
* ~~任务调度~~
* 系统调用
* 进程间通信
* 文件系统
* 运行应用程序
* 网络支持

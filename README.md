## Black OS
迷你操作系统内核

## Compile
```
sudo apt-get install build-essential nasm cgdb qemu
sudo ln -s /usr/bin/qemu-system-i386 /usr/bin/qemu
sudo mkdir /mnt/floppy
```
```
make
make qemu
```

## TODO
- [x] 启动
- [x] 字符打印
- [x] 中断服务
- [x] 键盘输入
- [x] 物理内存管理
- [x] 虚拟内存管理
- [x] 内核进程
- [x] 任务调度
- [ ] 系统调用
- [ ] 进程间通信
- [ ] 文件系统

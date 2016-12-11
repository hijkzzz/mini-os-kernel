# os-9527
![Build Status](https://img.shields.io/teamcity/codebetter/bt428.svg)

Mini Operating System Kernel

## Modules 
-  BootLoader 
-  VGA Driver
-  Interrupt Handlers
-  Keyboard Driver 
-  Physical Memory Management 
-  Virtual Memory Management
-  Kernel Process 
-  Process Scheduling

## Requirements
- Linux
- GCC
- NASM
- QEMU

## Build 
```
sudo apt-get install build-essential nasm gdb qemu
sudo ln -s /usr/bin/qemu-system-i386 /usr/bin/qemu
sudo mkdir /mnt/floppy
make
```

## Usage
```
make qemu
```

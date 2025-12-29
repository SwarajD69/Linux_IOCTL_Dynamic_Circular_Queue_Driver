# Linux Character Device with IOCTL Dynamic Circular Queue

## Overview

This project implements a Linux character device driver `/dev/swaraj_testing` that uses IOCTL calls to manage a dynamic circular queue in kernel space. The device supports blocking read operations, where a reader waits until data is available. User-space programs are provided to configure the queue, push data, and pop data.

## Device Node

```
/dev/swaraj_testing
```

## Demo Video

[https://drive.google.com/file/d/1t9p9d9l4N8W3QqDwy6AFOXaHm9Zif8kG/view?usp=sharing](https://drive.google.com/file/d/1t9p9d9l4N8W3QqDwy6AFOXaHm9Zif8kG/view?usp=sharing)

## Project Functionality

* Maintains a byte-level circular queue in kernel space
* User-space interaction via IOCTL calls
* Blocking POP operation when queue is empty
* Supports multiple producer and consumer processes
* Queue size configurable at runtime

## Features

* Dynamic queue size using `SET_SIZE_OF_QUEUE`
* Data push using `PUSH_DATA`
* Blocking data pop using `POP_DATA`
* Circular buffer with wrap-around support
* Wait-queue based blocking
* Kernel logs for open, close, push, and pop operations

## Project Structure

```
swaraj_testing_driver/
├── kernel/
│   ├── swaraj_testing.c
│   └── queue_ioctl.h
│   └── Makefile
├── user/
│   ├── configurator.c
│   ├── filler.c
│   ├── reader.c
│   └── queue_ioctl.h
│   └── Makefile
└── .gitignore
└── README.md
```

## Requirements

* Linux system
* Kernel headers
* GCC and Make
* Root privileges

## Install Dependencies

```
sudo apt install build-essential linux-headers-$(uname -r)
```

## Build Kernel Module

```
cd kernel
make clean
make
```

## Build User Programs

```
cd ../user
make
```

## Load Kernel Module

```
sudo insmod kernel/swaraj_testing.ko
```

## Verify Module

```
lsmod | grep swaraj_testing

or

ls /dev/swaraj_testing
```

## Create Device Node (If Required)

```
MAJOR=$(grep swaraj_testing /proc/devices | awk '{print $1}')
sudo mknod /dev/swaraj_testing c $MAJOR 0
sudo chmod 666 /dev/swaraj_testing
```

## Run User Programs

### Configure Queue Size

```
sudo ./user/configurator
dmesg | tail
```

### Push Data

```
sudo ./user/filler
dmesg | tail
```

### Pop Data (Blocking)

```
sudo ./user/reader
dmesg | tail
```

## Kernel Log Example

```
Device opened by process 4666
SET_SIZE_OF_QUEUE: initialized queue with size 100
queue_push: pushed 3 bytes, count=3, head=0, tail=3
queue_pop: popped 3 bytes, count=0, head=3, tail=3
Device closed by process 4666
```

## Notes

* PUSH operation is non-blocking
* POP operation blocks until data is available
* Circular queue wrap-around is handled correctly
* IOCTL structures are shared between kernel and user space
* Safe memory handling is implemented

## Unload Kernel Module

```
sudo rmmod swaraj_testing
```

## Author

Swaraj

Linux Kernel Module & IOCTL Programming Project
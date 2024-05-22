#!/bin/bash

make install CONFIG_PREFIX=/home/alientek/workspace/linux_raw/busybox/rootfs

cd ../rootfs/
pwd
tar -vcjf rootfs-alientek-emmc.tar.bz2 *

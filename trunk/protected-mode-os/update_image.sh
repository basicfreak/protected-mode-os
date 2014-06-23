#!/bin/bash

sudo /sbin/losetup /dev/loop0 floppy
sudo mount /dev/loop0 /mnt2
sudo cp SRC/KERNEL.ELF /mnt2/KERNEL.ELF
sudo umount /dev/loop0
sudo /sbin/losetup -d /dev/loop0

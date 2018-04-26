#!/bin/bash

sudo insmod ./MY_IPC.ko
lsmod |grep IPC

sudo chmod 666 /dev/myipc_if
ls -l /dev/myipc_if

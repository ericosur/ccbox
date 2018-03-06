#!/bin/sh
# http://www.felix-lin.com      date: 2013/07/04
#
# script for system V semaphore example
#

if [ ! -e /proc/sysvipc/sem ]; then
    echo "Does you kernel support System V IPC?"
    exit 1
fi

app1=./proc1
app2=./proc2

if [ -e $app1 ] ; then
    if [ -e $app2 ] ; then
        ${app1} &
        ${app2} &
        pid=`pidof ${app1}`
        wait ${pid}
    else
        echo "${app2} not found"
    fi
else
    echo "${app1} not found"
fi

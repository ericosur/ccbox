#!/bin/bash

export OMP_NUM_THREADS=2
export KMP_AFFINITY=compact,granularity=fine
export GLOG_minloglevel=3
./ssd_detect setting.json


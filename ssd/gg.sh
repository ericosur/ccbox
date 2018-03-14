#!/bin/bash

export OMP_NUM_THREADS=2
export KMP_AFFINITY=compact,granularity=fine
./ssd_detect setting.json


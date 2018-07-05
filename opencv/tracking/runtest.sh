#!/bin/bash
#

PAUSED=1
bounding='160,17,240,88'
tracker='build/tracker'

#tracker_algorithm can be: MIL, BOOSTING, MEDIANFLOW, TLD, KCF, GOTURN, MOSSE
# MOSSE acts strangely
# GOTURN needs caffe/model ready
# KCF needs opencl/gpu
# MEDIANFLOW is not supported
ALGORITHM='MIL BOOSTING TLD'

for cmd in ${ALGORITHM}
do
    echo "algorithm is ${cmd}"
    ${tracker} $cmd videos/chaplin.mp4 0 ${bounding}
    sleep $PAUSED
done

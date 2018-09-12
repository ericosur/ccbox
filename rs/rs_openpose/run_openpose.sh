#!/bin/bash

# put this script at top dir of bvlc/openpose
# make and build openpose (with example built)
# input raw depth and color image from folder $TOP/input_rs
# output json of key points to folder $TOP/output_json
#

#        --image_dir="examples/media/" \

appname='build/examples/openpose/openpose.bin'
if [ -f ${appname} ] ; then
    ${appname} \
        --image_dir="input_rs/" \
        --model_pose="COCO"       \
        --write_json="output_json" \
        --write_images="output_json"
else
    echo -e "executable not found, need build first"
fi

#!/bin/bash

TOP=${PWD}
ssd=ssd_detect

mkdir -p f && cd f && cmake .. && make -j2
if [ -e ${ssd} ] ; then
    echo "ok"
else
    echo "NG"
    exit 1
fi

cd $TOP

caffe_root=${HOME}/src/github/intel/caffe
caffe_lib=${caffe_root}/build/install/lib
intel_mklml=${caffe_root}/external/mkl/mklml_lnx_2018.0.1.20171227/lib
intel_mkldnn=${caffe_root}/external/mkldnn/install/lib

function is_dir_exist() {
    echo -e "check $1"
    if [ "$1" == "" ] ; then
        echo -e "parameter is empty\n"
        exit 1
    fi

    if [ -d $1 ] ; then
        result=1
    else
        result=0
        echo -e "dir $1 not exist..."
        exit 2
    fi
}


is_dir_exist ${caffe_root}
is_dir_exist ${caffe_lib}
is_dir_exist ${intel_mklml}
is_dir_exist ${intel_mkldnn}

LD_LIBRARY_PATH=${caffe_lib}:${intel_mmklml}:${intel_mkldnn}  ./f/ssd_detect setting.json

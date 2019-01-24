#!/usr/bin/env python3

'''
~/intel/wing/20181018_face_detection/fp32
Mobilenet_WIDER2_SSD_300x300_iter_160000.bin
Mobilenet_WIDER2_SSD_300x300_iter_160000.xml
'''

import os
import sys
import numpy as np
import time
import cv2
from openvino.inference_engine import IENetwork, IEPlugin


home = os.getenv('HOME')
wingdir = '/intel/wing/'
model_xml = home + wingdir + '20181018_face_detection/fp32/Mobilenet_WIDER2_SSD_300x300_iter_160000.xml'
model_bin = home + wingdir + '20181018_face_detection/fp32/Mobilenet_WIDER2_SSD_300x300_iter_160000.bin'

model_xml = home + wingdir + '20181018_face_detection/resnext/fp32/Resnext26125_WIDER_SSD_300x300_iter_90000.xml'
model_bin = home + wingdir + '20181018_face_detection/resnext/fp32/Resnext26125_WIDER_SSD_300x300_iter_90000.bin'

net = IENetwork.from_ir(model=model_xml, weights=model_bin)

#plugin = IEPlugin(device="HETERO:MYRIAD,CPU")
#plugin.set_config({"TARGET_FALLBACK": "HETERO:MYRIAD,CPU"})
#plugin.set_initial_affinity(net)

plugin = IEPlugin(device="CPU",plugin_dirs="/opt/intel/computer_vision_sdk/deployment_tools/inference_engine/lib/ubuntu_16.04/intel64/")
#plugin.set_initial_affinity(net)

if True:
    for l in net.layers.values():
        print("{} - {}: {}".format(l.name, l.type, l.affinity))

        # if l.type == "Convolution":
        #     l.affinity = "CPU"

exec_net = plugin.load(network=net)
input_blob = next(iter(net.inputs))
out_blob = next(iter(net.outputs))

del net

'''
RuntimeError: Unsupported primitive of type: PriorBox name: conv11_mbox_priorbox
'''

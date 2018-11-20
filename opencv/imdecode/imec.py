#!/usr/bin/env python2
# coding=utf-8

# refer to http://cctg.blogspot.tw/2017/07/socket-opencv-client.html

import os
import cv2
import numpy

ifn = '../data/lena600.jpg'
ofn = 'out.jpg'
tfn = 'out.txt'

if os.path.isfile(ifn):
    img = cv2.imread(ifn)
else:
    print('{} is not found, exit'.format(ifn))
    os.exit(1)

encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 90]
result, img_str = cv2.imencode('.jpg', img, encode_param)
#result, img_str = cv2.imencode('.jpg', img)
#print('img_str: {}'.format(img_str))
data = numpy.array(img_str)
stringData = data.tostring()

tfile = open(tfn, "w")
tfile.write(stringData)
tfile.close()

# now read from file and decode into image
rfile = open(tfn, "r")
img_str2 = rfile.read()
data = numpy.fromstring(img_str2, dtype='uint8')
decimg = cv2.imdecode(data, 1)
if len(img_str) == len(img_str2):
    print('size is same')
else:
    print('size not matched')
    exit()

cv2.imwrite(ofn, decimg)
#cv2.imshow("readback", readbackimg)
#cv2.waitKey(0)

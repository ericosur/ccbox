#!/usr/bin/env python2

import cv2

ifn = 'lena.jpg'
img = cv2.imread(ifn)
#cv2.imshow("img", img)
#cv2.waitKey(0)

img_str = cv2.imencode('.jpg', img)[1].tostring()

ofn = 'out.txt'
ofile = open(ofn, "wb")
ofile.write(img_str)
ofile.flush()
ofile.close()

# now read from file and decode into image
rfile = open(ofn, "rb")
img_str2 = rfile.read()
if len(img_str) == len(img_str2):
    print('size is same')
else:
    print('size not matched')
    exit()

readbackimg = cv2.imdecode(rfile, 0)
cv2.imshow("readback", readbackimg)
cv2.waitKey(0)


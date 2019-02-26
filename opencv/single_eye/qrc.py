#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import cv2 as cv
import myutil


det = cv.QRCodeDetector()


def draw_qrcode_contour(img, text, points, win_title='result'):
    #rgb(66, 134, 244)
    cv.drawContours(img, points, 0, (244, 134, 66), 2)
    cv.putText(img, text, (10, 40), cv.FONT_HERSHEY_SIMPLEX, 1, (127, 0, 255), 2, cv.LINE_AA)
    cv.imshow(win_title, img)


def run_detect(img):
    disp_img = img.copy()

    retval, points = det.detect(img)
    if retval:
        print("points: {}".format(points))
        draw_qrcode_contour(disp_img, [np.array(points, dtype=np.int32)], win_title=run_detect.__name__)
    else:
        print('not detected...')

def run_detect_decode(img, webcam=False):
    disp_img = img.copy()
    retval, points, straight_qrcode = det.detectAndDecode(img)
    if retval:
        print('retval: {}'.format(retval))
        draw_qrcode_contour(disp_img, retval, [np.array(points, dtype=np.int32)], win_title=run_detect_decode.__name__)
    else:
        if not webcam:
            print('no result')
    #print("points: {}".format(points))
    #print("sqrcode: {}".format(straight_qrcode))


def do_image(fn):
    print('read: {}'.format(fn))
    if not myutil.isfile(fn):
        print("image {} not found...".format(fn))
        exit(1)
    img = cv.imread(fn)
    #run_detect(img)
    run_detect_decode(img)


def main():
    use_image = False

    if use_image:
        fn = '../qrcode/qrcode.png'
        do_image(fn)
        return

    camera = cv.VideoCapture(0)
    while camera.isOpened():
        ret, frame = camera.read()
        if ret:
            cv.imshow('webcam', frame)
            run_detect_decode(frame, webcam=True)
            key = cv.waitKey(1)
            if key == 0x1B:
                break

    cv.destroyAllWindows()

if __name__ == '__main__':
    main()

#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
using qrcode detector of opencv to detect and decode QRCode
add a function that use PIL to draw text on image for CJK characters
'''

import sys
import cv2 as cv
from PIL import Image, ImageDraw, ImageFont
import numpy as np
import myutil


# pylint: disable=too-many-arguments
def add_text_to_image(img, text, left, top, textColor=(0, 255, 0), textSize=20):
    ''' using PIL to render text '''
    if isinstance(img, np.ndarray):
        img = Image.fromarray(cv.cvtColor(img, cv.COLOR_BGR2RGB))
    draw = ImageDraw.Draw(img)
    fontText = ImageFont.truetype(
        "NotoSansCJK-Regular.ttc", textSize, encoding="utf-8")
    draw.text((left, top), text, textColor, font=fontText)
    return cv.cvtColor(np.asarray(img), cv.COLOR_RGB2BGR)


def draw_qrcode_contour(img, text, points, win_title='result'):
    ''' draw qrcode contour '''
    #rgb(66, 134, 244)
    cv.drawContours(img, points, 0, (244, 134, 66), 2)
    #cv.putText(img, text, (10, 40), cv.FONT_HERSHEY_SIMPLEX, 1, (127, 0, 255),
    #           2, cv.LINE_AA)
    img = add_text_to_image(img, text, 10, 10)
    cv.imshow(win_title, img)


def run_detect(img):
    ''' run_detect '''
    disp_img = img.copy()
    det = cv.QRCodeDetector()
    retval, points = det.detect(img)
    if retval:
        print("points: {}".format(points))
        draw_qrcode_contour(disp_img, retval, [np.array(points, dtype=np.int32)],
                            win_title=run_detect.__name__)
    else:
        print('not detected...')

def run_detect_decode(img, webcam=False):
    ''' run_detect_decode '''
    disp_img = img.copy()
    det = cv.QRCodeDetector()
    #retval, points, straight_qrcode = det.detectAndDecode(img)
    retval, points, _ = det.detectAndDecode(img)
    if retval:
        print('retval: {}'.format(retval))
        draw_qrcode_contour(disp_img, retval, [np.array(points, dtype=np.int32)],
                            win_title=run_detect_decode.__name__)
    else:
        if not webcam:
            print('no result')
    #print("points: {}".format(points))
    #print("sqrcode: {}".format(straight_qrcode))


def do_image(fn):
    ''' qrcode from image '''
    print('read: {}'.format(fn))
    if not myutil.isfile(fn):
        print("image {} not found...".format(fn))
        sys.exit(1)
    img = cv.imread(fn)
    #run_detect(img)
    run_detect_decode(img)


def main():
    ''' main '''
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

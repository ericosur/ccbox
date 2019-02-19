#!/usr/bin/python3
# -*- coding: utf-8 -*-

#import the necessary packages
from glob import glob
import math
import numpy as np
import cv2
import checkerutil as ck

# checker board size 7 by 9
SIZE = (7, 9)

# size that restored from distortion
DST_W = 280
DST_H = 360


def main():
    window_name = 'capture'
    result_name = 'perspective'

    try:
        cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(window_name, 0, 0)
        cv2.namedWindow(result_name, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(result_name, 640, 0)

        cnt = 0

        # open webcam
        camera = cv2.VideoCapture(0)
        camera.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

        while camera.isOpened():
            # get a frame
            (grabbed, frame) = camera.read()
            image = frame.copy()

            #image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
            ret, crnrs = cv2.findChessboardCorners(image, SIZE, cv2.CALIB_CB_FAST_CHECK)
            #cv2.drawChessboardCorners(color_image, SIZE, crnrs, ret)

            if ret:
                fourc = ck.get_poi(crnrs)
                ck.draw_poi(image, fourc)

                cppts = ck.get_ppts(crnrs)
                M = ck.get_perspective_mat(cppts, DST_W, DST_H)
                perspective = cv2.warpPerspective(image, M, (DST_W, DST_H), cv2.INTER_LINEAR)
                cv2.imshow(result_name, perspective)

            # show a frame
            cv2.imshow(window_name, image)

            key = cv2.waitKey(1)
            if key & 0xFF == ord('q') or key == 0x1B:
                break
            elif key & 0xFF == ord('s'):
                fn = 'result-{:02d}.png'.format(cnt)
                print('save result to {}'.format(fn))
                cv2.imwrite(fn, color_image)
                cv2.imwrite('origin-{:02d}.png'.format(cnt), original)
                cnt += 1
            elif key == 0x20:
                cv2.waitKey(0)
    finally:
        cv2.destroyAllWindows()


if __name__ == '__main__':
    main()

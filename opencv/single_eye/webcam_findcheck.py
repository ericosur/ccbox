#!/usr/bin/python3
# -*- coding: utf-8 -*-

''' using webcam to find checker '''

#import the necessary packages
#from glob import glob
#import math
import cv2
#import numpy as np
import checkerutil as ck

# checker board size 7 by 9
SIZE = (7, 9)

# size that restored from distortion
DST_W = 280
DST_H = 360

WINDOW_NAME = 'capture'
RESULT_NAME = 'perspective'

def main():
    ''' main '''

    try:
        cv2.namedWindow(WINDOW_NAME, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(WINDOW_NAME, 0, 0)
        cv2.namedWindow(RESULT_NAME, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(RESULT_NAME, 640, 0)

        cnt = 0

        # open webcam
        camera = cv2.VideoCapture(0)
        camera.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

        while camera.isOpened():
            # get a frame
            (_, frame) = camera.read()
            image = frame.copy()
            original = image.copy()

            #image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
            ret, crnrs = cv2.findChessboardCorners(image, SIZE, cv2.CALIB_CB_FAST_CHECK)
            #cv2.drawChessboardCorners(color_image, SIZE, crnrs, ret)

            if ret:
                fourc = ck.get_poi(crnrs)
                ck.draw_poi(image, fourc)

                cppts = ck.get_ppts(crnrs)
                M = ck.get_perspective_mat(cppts, DST_W, DST_H)
                perspective = cv2.warpPerspective(image, M, (DST_W, DST_H), cv2.INTER_LINEAR)
                cv2.imshow(RESULT_NAME, perspective)

            # show a frame
            cv2.imshow(WINDOW_NAME, image)

            key = cv2.waitKey(1)
            if key & 0xFF == ord('q') or key == 0x1B:
                break
            if key & 0xFF == ord('s'):
                fn = 'result-{:02d}.png'.format(cnt)
                print('save result to {}'.format(fn))
                #cv2.imwrite(fn, color_image)
                cv2.imwrite(fn, image)
                cv2.imwrite('origin-{:02d}.png'.format(cnt), original)
                cnt += 1
            elif key == 0x20:
                cv2.waitKey(0)
    finally:
        cv2.destroyAllWindows()


if __name__ == '__main__':
    main()

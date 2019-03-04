#!/usr/bin/env python3

'''
there is also another add border example at
repo: ericosur-snippet/
path: opencv/python/add_border.py
'''

import os
import sys
import cv2

def get_default_image_path():
    home = os.environ["HOME"]
    im_pth = home + "/Pictures/data/avril1.jpg"
    return im_pth

def main(fn):
    desired_size = 700
    print('image path: {}'.format(fn))

    ## opencv has copyMakeBorder() method which is handy for making borders
    im = cv2.imread(fn)
    if im is None:
        print("failed to load image")
        return
    #print('shape: {}'.format(im.shape))
    old_size = im.shape[:2] # old_size is in (height, width) format
    ratio = float(desired_size)/max(old_size)
    new_size = tuple([int(x*ratio) for x in old_size])

    print('old_size:{} new_size:{}'.format(old_size, new_size))

    # new_size should be in (width, height) format
    im = cv2.resize(im, (new_size[1], new_size[0]))

    delta_w = desired_size - new_size[1]
    delta_h = desired_size - new_size[0]
    top, bottom = delta_h//2, delta_h-(delta_h//2)
    left, right = delta_w//2, delta_w-(delta_w//2)

    color = [0xcc, 0xaa, 0xbb]
    new_im = cv2.copyMakeBorder(im, top, bottom, left, right, cv2.BORDER_CONSTANT,
        value=color)

    cv2.imshow("image", new_im)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


if __name__ == '__main__':
    if len(sys.argv) > 1:
        fn = sys.argv[1]
    else:
        print('argv not specified, use default image path...')
        fn = get_default_image_path()
    main(fn)

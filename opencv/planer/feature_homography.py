#!/usr/bin/env python

'''
Feature homography
==================

Example of using features2d framework for interactive video homography matching.
ORB features and FLANN matcher are used. The actual tracking is implemented by
PlaneTracker class in plane_tracker.py

Inspired by http://www.youtube.com/watch?v=-ZNYoL8rzPY

video: http://www.youtube.com/watch?v=FirtmYcC0Vc

Usage
-----
feature_homography.py [<video source>]

Keys:
   SPACE  -  pause video

Select a textured planar object to track by drawing a box with a mouse.
'''

# Python 2/3 compatibility
from __future__ import print_function

import json
import numpy as np
import cv2 as cv

# local modules
import video
from video import presets
import common
from common import getsize, draw_keypoints
from plane_tracker import PlaneTracker

# my own util
import myutil

FRAME_FN = 'frame.jpg'

class App(object):
    def __init__(self, src):
        self.cap = video.create_capture(src, presets['book'])
        self.frame = None
        self.paused = False
        self.tracker = PlaneTracker()

        cv.namedWindow('plane')
        self.rect_sel = common.RectSelector('plane', self.on_rect)
        self.user_selected_rect = None

    def on_rect(self, rect):
        self.tracker.clear()
        self.tracker.add_target(self.frame, rect)
        # rasmus hacked {
        self.user_selected_rect = rect
        x0, y0, x1, y1 = rect
        crop_img = self.frame[y0:y1, x0:x1]
        #print('type: {}, {}, {}'.format(rect, type(rect), type(rect[0])))
        fn = FRAME_FN
        self.write_file(fn, self.frame)
        fn = 'rect.jpg'
        self.write_file(fn, crop_img)
        self.save_config(rect)
        # rasmus hacked }

    def save_config(self, rect):
        fn = FRAME_FN
        # elements in 'rect' is numpy.int16, convert them into int,
        # so that json could store
        rects = list(map(int, rect))
        data = {'fn': fn, 'rect': rects}
        print(data)
        j = json.dumps(data)    # now j is a string to keep json data
        with open('homo.json', 'w') as homo:
            homo.write(j)

    def load_config(self, fn):
        data = myutil.read_jsonfile(fn, debug=True)
        #print(data['fn'])
        fn = data.get('fn')
        frame = np.zeros((640, 480, 3), np.uint8)
        if myutil.isfile(fn):
            frame = cv.imread(fn)
        else:
            print('file not found: {}'.format(fn))
            return None

        rect = data['rect']
        rect = tuple(map(np.int16, rect))
        print(rect)
        self.tracker.clear()
        self.tracker.add_target(frame, rect)
        print('preset loaded')
        return frame

    @staticmethod
    def write_file(fn, frame):
        cv.imwrite(fn, frame)
        print('output to %s' % fn)


    def show_wtf(self, wtf):
        print('wtf: {} -- {}'.format(wtf, self.user_selected_rect))

    def check_wtf(self, wtf):
        for ww in wtf:
            xx, yy = ww
            if xx > 640 or yy > 480 or xx < 0 or yy < 0:
                return False
        return True

    def run(self):
        if True and myutil.isfile('homo.json'):
            print('preset exists')
            tmp_frame = self.load_config('homo.json')
            #self.frame = tmp_frame

        while True:
            playing = not self.paused and not self.rect_sel.dragging
            if playing or self.frame is None:
                ret, frame = self.cap.read()
                if not ret:
                    break
                self.frame = frame.copy()

            w, h = getsize(self.frame)
            vis = np.zeros((h, w*2, 3), np.uint8)
            # if tmp_frame:
            #     vis[:,w:] = tmp_frame
            vis[:h,:w] = self.frame
            if len(self.tracker.targets) > 0:
                target = self.tracker.targets[0]
                vis[:,w:] = target.image
                draw_keypoints(vis[:,w:], target.keypoints)
                x0, y0, x1, y1 = target.rect
                cv.rectangle(vis, (x0+w, y0), (x1+w, y1), (0, 255, 0), 2)

            if playing:
                tracked = self.tracker.track(self.frame)
                if len(tracked) > 0:
                    tracked = tracked[0]
                    wtf = np.int32(tracked.quad)
                    if self.check_wtf(wtf):
                        #self.show_wtf(wtf)
                        cv.polylines(vis, [np.int32(tracked.quad)], True, (255, 255, 255), 2)
                        for (x0, y0), (x1, y1) in zip(np.int32(tracked.p0), np.int32(tracked.p1)):
                            cv.line(vis, (x0+w, y0), (x1, y1), (0, 255, 0))

            draw_keypoints(vis, self.tracker.frame_points)

            self.rect_sel.draw(vis)
            cv.imshow('plane', vis)
            ch = cv.waitKey(1)
            if ch == ord(' '):
                self.paused = not self.paused
            if ch == 27:
                break

if __name__ == '__main__':
    #print(__doc__)

    import sys
    try:
        video_src = sys.argv[1]
    except:
        video_src = 0
    App(video_src).run()

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

import sys
import json
import math
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
WIN_NAME = 'plane'

class App(object):
    def __init__(self, src):
        cv.namedWindow(WIN_NAME)
        cv.moveWindow(WIN_NAME, 0, 0)
        self.cap = video.create_capture(src, presets['book'])
        self.frame = None
        self.paused = False
        self.tracker = PlaneTracker()
        self.rect_sel = common.RectSelector('plane', self.on_rect)
        self.user_selected_rect = None
        self.focal_length = 0.0
        self.horizontal_angel = 0.0
        self.KNOWN_WIDTH = 40
        self.KNOWN_HEIGHT = 40
        self.msg = None

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
        self.focal_length = self.calc_focallength(rect)

        rect = tuple(map(np.int16, rect))
        print(rect)
        self.tracker.clear()
        self.tracker.add_target(frame, rect)
        print('preset loaded')
        return frame

    def calc_focallength(self, rect):
        # initialize the known distance from the camera to the object, which
        # preset distance from samples
        KNOWN_DISTANCE = 200
        # initialize the known object width, which in this case, the piece of
        # paper, unit mm
        width = rect[2] - rect[0]
        focalLength = (width * KNOWN_DISTANCE) / self.KNOWN_WIDTH
        print('width: {} focal length: {:.2f}'.format(width, focalLength))
        return focalLength

    def distance_to_camera(self, knownWidth, focalLength, perWidth):
        # compute and return the distance from the maker to the camera
        if perWidth == 0.0:
            return 0.0
        return (knownWidth * focalLength) / perWidth

    @staticmethod
    def get_dist2d(p1, p2):
        return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)

    @staticmethod
    def write_file(fn, frame):
        cv.imwrite(fn, frame)
        print('output to %s' % fn)

    @staticmethod
    def get_slope(p1, p2):
        delta_x = float(p2[0] - p1[0])
        delta_y = float(p2[1] - p1[1])
        if delta_x == 0:
            return -1.0
        return math.fabs(delta_y / delta_x)

    @staticmethod
    def get_degree(slope):
        if slope < 0.0:
            return 90.0
        deg = math.atan(slope) * 180.0 / math.pi
        return deg

    def show_wtf(self, wtf):
        print('wtf: {} -- {}'.format(wtf, self.user_selected_rect))

    def check_wtf(self, wtf, debug=False):
        self.msg = None
        for ww in wtf:
            xx, yy = ww
            if xx > 640 or yy > 480 or xx < 0 or yy < 0:
                if debug:
                    print('OOB')
                return False
        ang1 = self.get_degree(self.get_slope(wtf[0], wtf[1]))
        ang2 = self.get_degree(self.get_slope(wtf[2], wtf[3]))
        if ang1 > 30.0 or ang2 > 30:
            if debug:
                print('h ang')
            return False

        self.horizontal_angel = ang1
        dist = self.get_dist2d(wtf[0], wtf[1])
        h_dist = dist * math.cos(self.horizontal_angel * math.pi / 180.0)
        detph = self.distance_to_camera(self.KNOWN_WIDTH, self.focal_length, h_dist)
        #print('ang{:4.2f} dist{:4.2f}'.format(self.horizontal_angel, dist), end=' ')
        #print('hdist {:4.2f}'.format(h_dist), end=' ')
        self.msg = 'depth {:4.2f} mm'.format(detph)

        s1 = self.get_slope(wtf[1], wtf[2])
        s2 = self.get_slope(wtf[0], wtf[3])
        if s1 < 0.0 or s2 < 0.0:
            if debug:
                print('t slopy')
            return False
        ang1 = self.get_degree(s1)
        ang2 = self.get_degree(s2)
        if ang1 < 60.0 or ang2 < 60.0:
            if debug:
                print('too s')
            return False

        return True


    def run(self):
        if True and myutil.isfile('homo.json'):
            print('preset exists')
            tmp_frame = self.load_config('homo.json')
            #self.frame = tmp_frame

        print('test: {}'.format(self.distance_to_camera(self.KNOWN_WIDTH, self.focal_length, 135)))

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

            is_ok_to_export = False
            if playing:
                tracked = self.tracker.track(self.frame)
                if len(tracked) > 0:
                    tracked = tracked[0]
                    wtf = np.int32(tracked.quad)
                    if self.check_wtf(wtf):
                        cv.putText(vis, self.msg, (50, 50), cv.FONT_HERSHEY_SIMPLEX, 1.0, (0,0,0), lineType=cv.LINE_AA)
                        cv.polylines(vis, [np.int32(tracked.quad)], True, (255, 255, 255), 2)
                        cv.fillPoly(vis, [np.int32(tracked.quad)], (255,0,0))
                        for (x0, y0), (x1, y1) in zip(np.int32(tracked.p0), np.int32(tracked.p1)):
                            cv.line(vis, (x0+w, y0), (x1, y1), (0, 255, 0))
                        is_ok_to_export = True


            draw_keypoints(vis, self.tracker.frame_points)

            self.rect_sel.draw(vis)
            cv.imshow(WIN_NAME, vis)
            ch = cv.waitKey(1)
            if ch == ord(' '):
                self.paused = not self.paused
            elif ch == 27:
                break
            elif ch == ord('s'):
                cv.imwrite('homo.png', vis)


def main():
    try:
        video_src = sys.argv[1]
    except:
        video_src = 0
    App(video_src).run()


if __name__ == '__main__':
    #print(__doc__)
    main()


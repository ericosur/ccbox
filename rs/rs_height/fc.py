#!/usr/bin/env python

''' find contour '''

import cv2
import numpy as np
from myutil import read_setting

'''
| distance (mm) |  pixel per mm |  D * P  |
|---------------|---------------|---------|
| 474           | 1.96          | 929.04  |
| 648           | 1.44          | 933.12  |
| 1004          | 0.96          | 963.84  |
| 1505          | 0.64          | 963.2   |
| ? 2000        | ?             | 999.0   |
if d < 1000, use 931.08
if d > 1000, use 963.5

'''

class FindContour(object):

    def __init__(self):
        self.colorfn = ''
        self.depthfn = ''
        self.raw_fh = None

    def load_file(self, color_fn, depth_fn):
        self.colorfn = color_fn
        self.depthfn = depth_fn
        self.raw_fh = open(depth_fn, "rb")

    def get_raw_u16(self, pos):
        ''' return depth value at specified position, unit in mm '''
        if pos > 1280 * 720 * 2:
            print('out of range')
            return 0
        self.raw_fh.seek(pos * 2)
        _mm = ord(self.raw_fh.read(1))
        _nn = ord(self.raw_fh.read(1))
        _hh = _nn * 256 + _mm
        return _hh


    @staticmethod
    def get_pos(x, y):
        WIDTH = 1280
        HEIGHT = 720
        return (y * WIDTH + x)


    def get_actual_length(self, width, distance):
        F = 999.0
        if distance > 2000:
            F = 945.0   # ???
        elif distance > 1000:
            F = 963.5
        else:
            F = 931.08
        ratio = distance / F
        _length = width * ratio     # unit mm
        return _length

    def get_contour(self):
        LOWER = 1000
        UPPER = 3000
        REPEAT = 10

        img = cv2.imread(self.colorfn)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        ret, binary = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)
        im2, contours, hierarchy = cv2.findContours(binary, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        # draw all contours
        #cv2.drawContours(img, contours, -1, (0,0,255), 3)

        for i, ctr in enumerate(contours):
            area = cv2.contourArea(ctr)
            # [min(x,y), max(x,y)]
            _mm = [[9999, 9999], [-1, -1]]
            _sum = 0
            if area > LOWER and area < UPPER:
                cv2.drawContours(img, contours, i, (128, 255, 0), 1)
                print('area for {} is {} '.format(i, area))
                for j, pts in enumerate(ctr):
                    #if j > REPEAT:
                    #    break
                    _x = pts[0][0]
                    _y = pts[0][1]
                    _pos = self.get_pos(_x, _y)
                    _depth = self.get_raw_u16(_pos)
                    _sum += _depth
                    #print('x{}, y{}, {}mm'.format(_x, _y, _depth))
                    if _x < _mm[0][0]:
                        _mm[0][0] = _x
                    if _x > _mm[1][0]:
                        _mm[1][0] = _x
                    if _y < _mm[0][1]:
                        _mm[0][1] = _y
                    if _y > _mm[1][1]:
                        _mm[1][1] = _y
                _avg = _sum / len(ctr)
                _bw = _mm[1][0] - _mm[0][0]
                _bh = _mm[1][1] - _mm[0][1]
                print('mm: {}, avg: {} bw:{},bh:{}'.format(_mm, _avg, _bw, _bh))
                _estlen = self.get_actual_length(_bw, _depth)
                dist_str = 'width: {:.2f}mm'.format(_estlen)
                pp = (_mm[1][0], _mm[1][1])
                cv2.putText(img, dist_str, pp, cv2.FONT_HERSHEY_SIMPLEX, 0.8,
                        (255, 255, 255), 2, cv2.LINE_AA)

        cv2.imshow('img', img)
        #cv2.imshow('im2', im2)
        cv2.waitKey(0)
        self.raw_fh.close()

    @staticmethod
    def get_pixel_distance(p1, p2):
        import math
        _dist = math.sqrt(pow(p1[0]-p2[0],2.0) + pow(p1[1]-p2[1],2.0))
        return _dist

    def query_length(self, p1, p2, img=None):

        if type(img) != type(None):
            has_input_img = True
            #print('fc.py has input image')
        else:
            print('read from self.colorfn')
            has_input_img = False
            img = cv2.imread(self.colorfn)

        dis1 = self.get_raw_u16(self.get_pos(p1[0], p1[1]))
        dis2 = self.get_raw_u16(self.get_pos(p2[0], p2[1]))

        pixel_dist = self.get_pixel_distance(p1, p2)
        avg = (dis1 + dis2) / 2.0
        print('avg pixel distance: {:.2f}'.format(avg))
        _estlen = self.get_actual_length(pixel_dist, (dis1+dis2)/2)
        print('estimated height: {:.2f}mm'.format(_estlen))
        dist_str = '{:.2f}mm'.format(_estlen)
        cv2.putText(img, dist_str, (10,20), cv2.FONT_HERSHEY_SIMPLEX, 0.75,
                    (255, 255, 255), 2, cv2.LINE_AA)

        dist_str = '{}'.format(dis1)
        cv2.circle(img, p1, 3, color=(255, 255, 255), thickness=3)
        cv2.putText(img, dist_str, p1, cv2.FONT_HERSHEY_SIMPLEX, 0.75,
                    (255, 255, 255), 2, cv2.LINE_AA)
        dist_str = '{}'.format(dis2)
        cv2.circle(img, p2, 3, color=(255, 255, 255), thickness=3)
        cv2.putText(img, dist_str, p2, cv2.FONT_HERSHEY_SIMPLEX, 0.75,
                    (255, 255, 255), 2, cv2.LINE_AA)

        cv2.line(img, p1, p2, color=(255,255,255), thickness=2)

        if not has_input_img:
            print('imshow from fc.py')
            cv2.imshow('img', img)
            cv2.waitKey(0)

        return img

    def query_length_without_img(self, p1, p2, show=True):
        ''' return estimated length for p1, p2
            if show is Ture will print to terminal
        '''
        dis1 = self.get_raw_u16(self.get_pos(p1[0], p1[1]))
        dis2 = self.get_raw_u16(self.get_pos(p2[0], p2[1]))

        pixel_dist = self.get_pixel_distance(p1, p2)
        avg = (dis1 + dis2) / 2.0
        #print('avg pixel distance: {:.2f}'.format(avg))
        _estlen = self.get_actual_length(pixel_dist, (dis1+dis2)/2)
        if show:
            print('{:.2f}mm, '.format(_estlen))
        return _estlen


def test_with_setting(jsondata, item):
    sett = jsondata[item]
    #print('{},{}'.format(sett['color'], sett['depth']))
    try:
        base_dir = jsondata['base_dir']
        color_path = base_dir + '/' + sett['color']
        depth_path = base_dir + '/' + sett['depth']
    except KeyError:
        print('KeyError')
        return

    myctr = FindContour()
    myctr.load_file(color_path, depth_path)
    p1 = (sett['p1']['x'], sett['p1']['y'])
    p2 = (sett['p2']['x'], sett['p2']['y'])
    print('p1:{}, p2:{}'.format(p1, p2))
    myctr.query_length_without_img(p1, p2)

def test_proc():
    json = read_setting('setting.json')
    #print(json['base_dir'])
    for ii in range(json['min'], json['max']):
        #print('ii: {}'.format(ii))
        test_with_setting(json, str(ii))

def main():
    myctr = FindContour()
    #myctr.load_file('sn0001_Color.png', 'sn0001_Depth.raw')
    #myctr.load_file('sn0002_Color.png', 'sn0002_Depth.raw')
    #myctr.load_file('ian03_Color.png', 'ian03_Depth.raw')
    myctr.load_file('1_Depth.png', '1_Depth.raw')
    p1 = (500, 200)
    p2 = (550, 650)
    myctr.query_length_without_img(p1, p2)

if __name__ == '__main__':
    #main()
    test_proc()

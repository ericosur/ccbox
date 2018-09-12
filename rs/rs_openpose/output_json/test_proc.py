#!/usr/bin/env python3

'''
list COCO keypoints from json file
'''

import sys
from glob import glob

from myutil import read_jsonfile, isfile
from fc import FindContour
from kp import KP


class TestProc(object):
    def __init__(self, prefix, show_image=False):
        self.prefix = prefix
        self.flag_show_image = show_image
        if self.flag_show_image:
            import cv2
            self.imgfont = cv2.FONT_HERSHEY_SIMPLEX
        else:
            self.imgfont = None
        orig_path = '../input_rs/'
        self.imgfile = orig_path + prefix + '_Color.png'
        self.rawfile = orig_path + prefix + '_Depth.raw'
        self.jfile = prefix + '_Color_keypoints.json'
        self.ljson = 'links.json'
        self.proceed = False
        if not self.check_files():
            print('missing files...')
            return
        else:
            self.proceed = True

        linkjson = read_jsonfile(self.ljson)
        self.links = linkjson['link']
        self.json = read_jsonfile(self.jfile)



    def check_files(self):
        if not isfile(self.imgfile):
            print('{} not found'.format(self.imgfile))
            return False
        if not isfile(self.jfile):
            print('{} not found'.format(self.jfile))
            return False
        if not isfile(self.rawfile):
            print('{} not found'.format(self.rawfile))
            return False
        if not isfile(self.ljson):
            print('{} not found'.format(self.ljson))
            return False
        return True

    @staticmethod
    def rrt(tok):
        if type(tok) == str:
            return tok.replace('KP.', '')
        else:
            return str(tok).replace('KP.', '')

    def process_one_section(self):
        if not self.proceed:
            print('cannot proceed...')
            return

        myctr = FindContour()
        myctr.load_file(self.imgfile, self.rawfile)
        p1 = ()
        p2 = ()

        # TODO: if more than one person
        kps = self.json['people'][0]['pose_keypoints_2d']

        if self.flag_show_image:
            import cv2
            img = cv2.imread(self.imgfile)

        FROM_PT = KP.RIGHT_SHOULDER
        END_PT = KP.LEFT_SHOULDER
        print("{} to {}: ".format(FROM_PT, END_PT), end='')
        for idx, kpn in enumerate(KP):
            x = int(kps[idx * 3])
            y = int(kps[idx * 3 + 1])
            pos = (x, y)
            if self.flag_show_image:
                cv2.circle(img, pos, 3, color=(255, 255, 0))
                imgtext = str(kpn).replace('KP.', '')
                #print('{}: {} - ({},{})'.format(idx, imgtext, x, y))
                cv2.putText(img, imgtext, pos, self.imgfont, 0.8, (0, 0, 255), 2, cv2.LINE_AA)
            if idx == FROM_PT.value:
                p1 = (x, y)
            if idx == END_PT.value:
                p2 = (x, y)

        #print('show #1')
        #cv2.imshow('img', img)
        #cv2.waitKey(0)

        if self.flag_show_image:
            #print('imshow from listpts.py')
            img2 = myctr.query_length(p1, p2, img)
            cv2.imshow('img', img2)
            cv2.waitKey(0)
        else:
            myctr.query_length_without_show(p1, p2)


    def process_all_sections(self):
        if not self.proceed:
            print('cannot proceed...')
            return

        myctr = FindContour()
        myctr.load_file(self.imgfile, self.rawfile)
        p1 = ()
        p2 = ()

        # TODO: if more than one person
        kps = self.json['people'][0]['pose_keypoints_2d']

        flag_show_image = False

        # print header
        '''
        for ll in self.links:
            #print('{} - {}'.format(ll[0], ll[1]))
            print('{}({}) - {}({})'.format(
                TestProc.rrt(KP(ll[0])),
                ll[0],
                TestProc.rrt(KP(ll[1])),
                ll[1]),)
        '''

        # print data
        for ll in self.links:
            #print('{} - {}'.format(ll[0], ll[1]))
            x1 = int(kps[TestProc.get_x(ll[0])])
            y1 = int(kps[TestProc.get_y(ll[0])])
            x2 = int(kps[TestProc.get_x(ll[1])])
            y2 = int(kps[TestProc.get_y(ll[1])])
            p1 = (x1, y1)
            p2 = (x2, y2)
            myctr.query_length_without_show(p1, p2)


    @staticmethod
    def get_x(idx):
        return idx * 3

    @staticmethod
    def get_y(idx):
        return idx * 3 + 1

    @staticmethod
    def get_prefix(filepath):
        pref = filepath.replace('_Color_keypoints.json', '')
        return pref

if __name__ == '__main__':
    if len(sys.argv) == 1:
        files = glob('*Color_keypoints.json')
        for ff in files:
            print('process file: {}'.format(ff))
            prefix = TestProc.get_prefix(ff)
            tp = TestProc(prefix, show_image=False)
            tp.process_one_section()
            print()
    elif len(sys.argv) == 2:
        tp = TestProc(sys.argv[1])
        tp.process_one_section()

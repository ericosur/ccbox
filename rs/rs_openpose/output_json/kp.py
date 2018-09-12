#!/usr/bin/env python3

''' python3 enum '''

from enum import Enum


class KP(Enum):
    ''' it is COCO section enum '''

    NOSE = 0
    NECK = 1
    RIGHT_SHOULDER = 2
    RIGHT_ELBOW = 3
    RIGHT_WRIST = 4
    LEFT_SHOULDER = 5
    LEFT_ELBOW = 6
    LEFT_WRIST = 7
    RIGHT_HIP = 8
    RIGHT_KNEE = 9
    RIGHT_ANKLE = 10
    LEFT_HIP = 11
    LEFT_KNEE = 12
    LEFT_ANKLE = 13
    RIGHT_EYE = 14
    LEFT_EYE = 15
    RIGHT_EAR = 16
    LEFT_EAR = 17


def main():
    print('list all data member in enum KP ===>')
    for kk in KP:
        print(kk)

    print()
    TEST_IDX = 10
    print('what is KP({})? {}'.format(TEST_IDX, KP(TEST_IDX)), end='')
    print(' and its value is', KP(TEST_IDX).value)
    print('what is value of KP.LEFT_EAR?', KP.LEFT_EAR.value)


if __name__ == '__main__':
    main()

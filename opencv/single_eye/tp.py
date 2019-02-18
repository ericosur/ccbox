#!/usr/bin/env python3

# check 99.distortion.md

import cv2
import numpy as np

def get_perspective_mat():
    src_points = np.array([[270., 69.], [690.,164.], [273.,458.], [694.,518.]], dtype = "float32")
    dst_points = np.array([[0.,0.], [479.,0.], [0.,399.], [479.,399.]], dtype = "float32")
    M = cv2.getPerspectiveTransform(src_points, dst_points)
    return M

def main():
    fn = "Street_Sign_1200x675.jpg"
    image = cv2.imread(fn)
    # caller side
    M = get_perspective_mat()
    print("M: {}".format(M))
    perspective = cv2.warpPerspective(image, M, (480, 400), cv2.INTER_LINEAR)

    cv2.imshow("input", image)
    cv2.imshow("output", perspective)

    key = cv2.waitKey(0)
    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()

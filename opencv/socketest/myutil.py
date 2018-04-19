import json
import cv2
import numpy

def read_jsonfile(fn):
    print('load json from {}'.format(fn))
    data = json.load(open(fn))
    return data

def translate_img_to_str(fn):
    img = cv2.imread(fn)
    encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 90]
    result, img_str = cv2.imencode('.jpg', img, encode_param)
    #result, img_str = cv2.imencode('.jpg', img)
    #print('img_str: {}'.format(img_str))
    data = numpy.array(img_str)
    stringData = data.tostring()
    return stringData

def main():
    strData = translate_img_to_str('lena.jpg')
    print strData

if __name__ == '__main__':
    main()
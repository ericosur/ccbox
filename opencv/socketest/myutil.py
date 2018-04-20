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

def combine_two_images(ofn, inf1, inf2):
    img1 = cv2.imread(inf1)
    img2 = cv2.imread(inf2)

    new_height = max(img1.shape[0], img2.shape[0])
    new_width = max(img1.shape[1], img2.shape[1])
    if img1.shape[0] > img2.shape[0]:
        new_img = cv2.resize(img2, (new_height, new_width))
        img2 = new_img
    elif img1.shape[0] < img2.shape[0]:
        new_img = cv2.resize(img1, (new_height, new_width))
        img1 = new_img

    # axis = 1, left is img1, and right is img2
    # need have same height
    vis = numpy.concatenate((img1, img2), axis=1)
    cv2.imwrite(ofn, vis)


def main():
    # strData = translate_img_to_str('lena.jpg')
    # print strData
    combine_two_images('out.jpg', 'l128.jpg', 'l200.jpg')

if __name__ == '__main__':
    main()
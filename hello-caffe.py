# -*- coding: utf-8 -*-

'''
python2 hello-caffe.py
---or---
python3 hello-caffe.py
'''

from __future__ import print_function
import os, sys

def hello_world():
    import caffe
    print('pycaffe version: {}'.format(caffe.__version__))

def insert_syspath(p):
    if is_path_exist(p):
        print('insert {} into sys path'.format(p))
        sys.path.insert(0, p)
        return True
    return False

def get_python_version():
    ''' return version string of python '''
    py_ver = ".".join(map(str, sys.version_info[:2]))
    return py_ver

def is_path_exist(p):
    return os.path.exists(p)

def init():
    env_caffe_root = os.environ.get('CAFFE_ROOT')
    if not env_caffe_root:
        print('no CAFFE_ROOT')
        return False
    pycaffe_path = '{}/python'.format(env_caffe_root)
    if insert_syspath(pycaffe_path):
        print('insert syspath successfully'.format(pycaffe_path))
        return True

    #print('maybe {} not found?'.format(pycaffe_path))
    home_path = os.environ['HOME']
    #try_path = 'src/caffe-try'
    intel_path = 'src/github/intel/caffe'
    pycaffe_path = '{}/{}/python'.format(home_path,intel_path)
    if insert_syspath(pycaffe_path):
        print('2nd try insert syspath successfully'.format(pycaffe_path))
        return True

    return False

def main():
    if init():
        print('import pycaffe successfully')
    else:
        print('failed to import pycaffe...')
        return

    print('Would you like to call caffe function? will core dump a lot')
    ans = 'no'
    ver = float(get_python_version())
    #print('ver: ', get_python_version())
    if ver > 3.0:
        ans = input('enter "yes" to continue: ')
    else:
        ans = raw_input('enter "yes" to continue: ')
    if ans == 'yes':
        hello_world()


if __name__ == '__main__':
    main()

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

def is_path_exist(p):
    return os.path.exists(p)

def init():
    env_caffe_root = os.environ['CAFFE_ROOT']
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
        hello_world()
    else:
        print('failed to import pycaffe...')

if __name__ == '__main__':
    main()

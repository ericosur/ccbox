import os, sys

def hello_world():
    import caffe
    print 'hello world'

def insert_syspath(p):
    if is_path_exist(p):
        print('insert {} into sys path'.format(p))
        sys.path.insert(0, p)
    return

def is_path_exist(p):
    return os.path.exists(p)

def main():
    env_caffe_root = os.environ['CAFFE_ROOT']
    pycaffe_path = '{}/python'.format(env_caffe_root)
    if is_path_exist(pycaffe_path):
        insert_syspath(pycaffe_path)
        return

    home_path = os.environ['HOME']
    #try_path = 'src/caffe-try'
    intel_path = 'src/github/intel/caffe'
    pycaffe_path = '{}/{}/python'.format(home_path,intel_path)
    insert_syspath(caffe_root)
    return


if __name__ == '__main__':
    main()


'''
refer from: https://gist.github.com/kevinkindom/108ffd675cb9253f8f71
'''

# socket client

import socket
import sys
import time
from sockutil import read_jsonfile, translate_img_to_str

def save_data(data):
    f = open('p.dat', 'w')
    f.write(data)
    f.close()

def main(argv):
    data = read_jsonfile('setting.json')

    HOST = data['host']
    PORT = data['port']
    max_recv_size = data['max_recv_size']

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))

    for fn in argv:
        print("sending fn: {}".format(fn))
        strData = translate_img_to_str(fn)
        dataLen = str(len(strData)).ljust(16);
        print('dataLen: {}'.format(dataLen))
        s.send(dataLen)
        s.send(strData)
        #save_data(strData)
        while True:
            rxData = s.recv(max_recv_size)
            print('rxData: {}'.format(rxData))
            if rxData == 'ok':
                break
            else:
                time.sleep(0.5)

    print('close connection')
    s.close()
    return

if __name__ == '__main__':
    print('client.py')
    if len(sys.argv) > 1:
        main(sys.argv[1:])
    else:
        #arr = ['lena.jpg', 'test.jpg']
        arr = ['lena.jpg']
        main(arr)

#!/usr/bin/python

# refer from: https://gist.github.com/kevinkindom/108ffd675cb9253f8f71

# socket server

import socket
import myutil
import numpy
import cv2
import time

def recvall(sock, count):
    print('recvall, count:{}'.format(count))
    buf = b''
    while count:
        newbuf = sock.recv(count)
        if not newbuf:
            print('if not newbuf')
            return None
        buf += newbuf
        count -= len(newbuf)
    return buf


def main():
    data = myutil.read_jsonfile('setting.json')

    HOST = data['host']
    PORT = data['port']
    MAX_CONNECTION = data['max_connection']
    max_recv_size = data['max_recv_size']
    ofn = 'out.jpg'

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((HOST, PORT))
        s.listen(MAX_CONNECTION)
    except socket.error:
        return

    print 'Server start at: %s:%s' %(HOST, PORT)
    print 'wait for connection...'

    cnt = 0
    while True:
        conn, addr = s.accept()
        print 'Connected by ', addr

        while True:
            length = recvall(conn, 16)
            if length == None:
                break
            print('will recv length: {}'.format(length))
            strData = recvall(conn, int(length))
            if strData == None:
                print('strData is None')
                break
            data = numpy.fromstring(strData, dtype='uint8')
            decimg = cv2.imdecode(data, 1)
            ofn = '/tmp/ofn{0:04d}.jpg'.format(cnt)
            cv2.imwrite(ofn, decimg)
            print('imwrite: {}'.format(ofn))
            cnt += 1
            # busy doing something...
            print('wait 500ms')
            time.sleep(0.5)
            print('send ok...')
            conn.send("ok")

    conn.close()

if __name__ == '__main__':
    main()

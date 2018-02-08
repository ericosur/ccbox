/// file: simplenotify.cpp

#include "simplenotify.h"

#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <errno.h>

void print_errno(int err)
{
    std::string msg;
    switch (err) {
    case EACCES:
        msg = "EACCES";
        break;
    case EBADF:
        msg = "EBADF";
        break;
    case EFAULT:
        msg = "EFAULT";
        break;
    case EINVAL:
        msg = "EINVAL";
        break;
    case ENAMETOOLONG:
        msg = "ENAMETOOLONG";
        break;
    case ENOENT:
        msg = "ENOENT";
        break;
    case ENOMEM:
        msg = "ENOMEM";
        break;
    case ENOSPC:
        msg = "ENOSPC";
        break;
    }
    std::cerr << "errno: " << msg << "\n";
}


int wait_for_file(const std::string& watch_file)
{
    int fd;
    int wd;
    //int length;
    const int BUF_LEN = 64;
    char buffer[BUF_LEN];

    fd = inotify_init();
    if ( fd < 0 ) {
        std::cerr << "SimpleNotify: inotify_init failed\n";
        return fd;
    }

    int flag = IN_MODIFY | IN_CREATE | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE;
    wd = inotify_add_watch( fd, watch_file.c_str(), flag);
    if (wd < 0) {
        std::cerr << "SimpleNotify: inotify_add_watch failed, errno: " << errno;
        std::cerr << " at: " << watch_file << "\n";
        print_errno(errno);
        return wd;
    }

    // will block at read() here
    ssize_t readbyte = read( fd, buffer, BUF_LEN );
    (void)readbyte;

    (void) inotify_rm_watch(fd, wd);
    (void) close(fd);
    return 0;
}

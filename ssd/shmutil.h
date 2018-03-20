#ifndef __UTIL_SHM_UTIL_H__
#define __UTIL_SHM_UTIL_H__

#include <string>
#include <cstdint>

#define SHMKEY          0x0111C0DE
#define MAX_BUFFER_SIZE   (640*480*3)

#define MESGQKEY        0x6789CAFE
#define MAX_SEND_SIZE      512
#define MESSAGE_TYPE       9

typedef uint8_t byte;

struct mymsgbuf {
    long mtype;
    char mtext[MAX_SEND_SIZE];
};


struct shm_st {
    uint32_t size;
    byte buffer[MAX_BUFFER_SIZE];
};

void remove_shm();
bool read_from_shm(uint32_t& size, void* buffer);

std::string wait_msgq();
void remove_msgq();



#endif  // __UTIL_SHM_UTIL_H__

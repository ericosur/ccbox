#include "shmutil.h"
#include "ssd_setting.h"

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

extern SsdSetting settings;

void remove_shm()
{
    if (settings.show_debug){
        printf("%s\n", __func__);
    }

    int shm_id;
    // get id of shm
    shm_id = shmget((key_t)SHMKEY, sizeof(shm_st), 0600 | IPC_EXCL);
    if (shm_id == -1) {
        perror("shmget");
        return;
    }
    // destroy shared memory
    if (shmctl(shm_id, IPC_RMID, 0) == -1) {
        perror("shmctl");
        return;
    }
}

bool read_from_shm(uint32_t& size, void* buffer)
{
    int shm_id;
    shm_st *st;
    // get the ID of shared memory,
    // fail if specified shmem not created
    shm_id = shmget((key_t)SHMKEY, sizeof(shm_st), 0600 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget");
        return false;
    }
    // attach shared memory
    st = (shm_st*)shmat(shm_id, (void *)0, 0);
    if (st == (void *)-1) {
        perror("shmget");
        return false;
    }
    size = st->size;
    memcpy(buffer, st->buffer, size);
    // detach shared memory
    if (shmdt(st) == -1) {
        perror("shmdt");
        return false;
    }
    return true;
}


std::string wait_msgq()
{
    key_t key = MESGQKEY;
    int msgqueue_id;
    struct mymsgbuf qbuf;
    std::string result;

    //key = ftok(MSGQ_FILE, 'm');
    int type = MESSAGE_TYPE;
    if((msgqueue_id = msgget(key, IPC_CREAT|0660)) == -1) {
        perror("msgsnd: msgget");
        return "error"; // error here
    }
    if (settings.show_debug) {
      printf("msgq key:0x%08x\n", msgqueue_id);
    }
    while ( true ) {
        qbuf.mtype = MESSAGE_TYPE;
        int ret = msgrcv(msgqueue_id, (struct mymsgbuf *)&qbuf, MAX_SEND_SIZE, type, 0);
        if (ret > 0) {
          printf("recv text: %s\n", qbuf.mtext);
          result = qbuf.mtext;
          break;
        }
        usleep(250*1000);
    }
    return result;
}

void remove_msgq()
{
    int msg_id = msgget(MESGQKEY, IPC_EXCL|0660);
    if(msg_id == -1) {
        //perror("msgget");
        // and exit...
    } else if (msgctl(msg_id, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        // and exit...
    } else {
      fprintf(stderr, "remove_msgq: msgq used by ssd is removed\n");
    }
}


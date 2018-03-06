#ifndef	_SEMAPHORE_H
#define	_SEMAPHORE_H

#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#define KEY_SEM_EXAMPLE             0x0400


#define SEM_P(x)            semaphore_p(x)
#define SEM_V(x)            semaphore_v(x)

int init_semaphore(key_t key, int sem_val);
int del_semaphore(int sem_id);
int semaphore_p(int sem_id);
int semaphore_v(int sem_id);


#endif /* _SEMAPHORE_H */

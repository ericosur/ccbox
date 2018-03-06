
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include "semaphore.h"

/*
    desc: 
        get the semaphore and initiate it
    parameters:
        key - key value of semaphore
        sem_val - initial value of the semaphore. ignore if less the zero
    retrun:
        semaphore ID if successful. otherwise, return -1
*/
int init_semaphore(key_t key, int sem_val)
{
    int         sem_id;
    union semun sem_union;

    sem_id = semget(key, 1, 0666 | IPC_CREAT);
    
    if (sem_val >= 0) {
        sem_union.val = sem_val;
    
        if (semctl(sem_id, 0, SETVAL, sem_union) == -1) {
            return -1;
        }
    }
    
    return sem_id;
}

/*
    desc:
        delete the semaphore by ID
*/
int del_semaphore(int sem_id)
{
    union semun sem_union;

    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1) {
        return -1;
    } else {
        return 0;
    }
}

/*
    desc:
        wait for the semaphore by ID
*/
int semaphore_p(int sem_id)
{
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = -1;
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return -1;
    }
    return 0;
}

/*
    desc:
        signal the semaphore by ID
*/
int semaphore_v(int sem_id)
{
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = 1;
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        return -1;
    }
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <unistd.h>

#include <pbox/pbox.h>
#include "semaphore.h"

#define PROCESS_NAME        "foo"
#define SLEEP_DURING        (3)
#define WAIT_DURING         (10)

int main (int argc, char *argv[])
{
    using namespace pbox;

    int semid;

    //printf("%s - get the semaphore\n", PROCESS_NAME);
    mylog(PROCESS_NAME, "get semaphore");

    semid = init_semaphore(KEY_SEM_EXAMPLE, 1);
    if (semid == -1) {
        perror(PROCESS_NAME);
        exit(EXIT_FAILURE);
    }

    mylog(PROCESS_NAME, "wait the semaphore");
    if (SEM_P(semid) != 0) {
        perror(PROCESS_NAME);
        exit(EXIT_FAILURE);
    }

    mylog(PROCESS_NAME, "enter critical section, and sleep %d secs...", SLEEP_DURING);
    sleep(SLEEP_DURING);
    mylog(PROCESS_NAME, "exit critical section...");

    mylog(PROCESS_NAME, "signal the semaphore");
    SEM_V(semid);

    mylog(PROCESS_NAME, "wait for %d sec", WAIT_DURING);
    sleep(WAIT_DURING);

    mylog(PROCESS_NAME, "destory the semaphore");
    if (del_semaphore(semid) != 0) {
        perror(PROCESS_NAME);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

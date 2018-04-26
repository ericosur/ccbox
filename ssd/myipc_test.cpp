#include "MY_IPC.hpp"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void my_handle_ctrlc(int sig)
{
  printf("%s: caught signal: %d\n", __func__, sig);
  exit(1);
}


int main()
{
    signal(SIGINT, my_handle_ctrlc);

    MY_IPC* ipc = MY_IPC::MY_IPC_GetInstance();
    if (ipc == NULL) {
        printf("myipc_test: failed to init MY_IPC\n");
        return -1;
    }

    while (true) {
        int vol_tag = ipc->IPC_Get_TAG_INT32("audience_vol_tag");
        printf("vol_tag: %d\n", vol_tag);
    }



    return 0;
}

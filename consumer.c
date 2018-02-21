//
// Created by fof_z on 2/19/2018.
//

#include <stdio.h>
#include <unistd.h>
#include "share.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
    iint sharekey, shareid;
    struct share * shares;
    printf("Hello, I'm consumer process %d and my parent is process %d\n", getpid(), getppid());

    sharekey = atoi(argv[1]);
    shareid = shmget(sharekey, sizeof(struct share), 0777);

    shares = (struct share *)shmat(shareid, NULL, 0);

    shmdt(shares);
    printf("Shared memory has been detached.\n");
    return 0;
}
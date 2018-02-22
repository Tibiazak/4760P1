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
    FILE * fp;
    int sharekey, shareid;
    struct share * shares;
    printf("Hello, I'm producer process %d and my parent is process %d\n", getpid(), getppid());

    sharekey = atoi(argv[1]);
    shareid = shmget(sharekey, sizeof(struct share), 0777);

    shares = (struct share *)shmat(shareid, NULL, 0);

    fp = fopen("input", "r");

    if (fp == NULL)
    {
        perror("Problem opening Input!");
        shmdt(shares);
        exit(1);
    }

    if(shares->bufFlag[0] == empty)
    {
        fgets(shares->buf0, 20, fp);
        shares->bufFlag[0] = full;
    }

    fclose(fp);

    shmdt(shares);
    printf("Shared memory has been detached.\n");
    return 0;
}
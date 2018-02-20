//
// Created by fof_z on 2/19/2018.
//

#include <stdio.h>
#include <unistd.h>
#include "flags.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char * argv[])
{
    int flagkey, turnkey, flagid, turnid;
    int * turn;
    struct flags * flag;
    printf("Hello, I'm producer process %d and my parent is process %d\n", getpid(), getppid());

    flagkey = argv[1];
    turnkey = argv[2];
    flagid = shmget(flagkey, sizeof(struct flags), 0777);
    turnid = shmget(turnkey, sizeof(int), 0777);

    turn = (int *)shmat(turnid, NULL, 0);
    flag = (struct flags *)shmat(flagid, NULL, 0);

    printf("Flag is currently set to: %d, turn is: %d\n", flag->status, *turn);
    shmdt(turn);
    shmdt(flag);
    printf("Shared memory has been detached.\n");
    return 0;
}
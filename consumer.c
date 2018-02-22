/*
 * Joshua Bearden
 * 2/21/18
 * Operating Systems Project 2
 *
 * A Program to make a consumer process that will read a full buffer from shared memory,
 * and print that buffer to the log file, then read from another buffer; while enforcing
 * mutual exclusion using Peterson's Multiple Process Solution.
 *
 * This program is not complete! Currently it attaches shared memory and then reads a full
 * buffer, printing to the screen.
 */

#include <stdio.h>
#include <unistd.h>
#include "share.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
    // Initialize variables
    int sharekey, shareid;
    struct share * shares;
    printf("Hello, I'm consumer process %d and my parent is process %d\n", getpid(), getppid());

    // Attach shared memory
    sharekey = atoi(argv[1]);
    shareid = shmget(sharekey, sizeof(struct share), 0777);

    shares = (struct share *)shmat(shareid, NULL, 0);

    // Wait for the buffer to be full
    while(shares->bufFlag[0] != full);

    // Once the buffer is full, print the string from the buffer
    printf(shares->buf0);

    // Detach shared memory
    shmdt(shares);
    printf("Shared memory has been detached.\n");
    return 0;
}
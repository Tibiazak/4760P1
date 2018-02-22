/*
 * Joshua Bearden
 * 2/21/18
 * Operating Systems Project 2
 *
 * A Program to get some shared memory, and fill 5 buffers with
 * text from an input file; while enforcing mutual exclusion on the buffers
 * using Peterson's Multiple Process Solution.
 *
 * This program is incomplete, currently it opens the file, attaches shared memory,
 * fills a buffer, then closes the file and detaches shared memory.
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
    FILE * fp;
    int sharekey, shareid;
    struct share * shares;
    printf("Hello, I'm producer process %d and my parent is process %d\n", getpid(), getppid());

    // Get the shared memory key and attach shared memory
    sharekey = atoi(argv[1]);
    shareid = shmget(sharekey, sizeof(struct share), 0777);

    shares = (struct share *)shmat(shareid, NULL, 0);

    // Opens the input file
    fp = fopen("input", "r");

    // Ensure the file opened successfully, error and quit if not.
    if (fp == NULL)
    {
        perror("Problem opening Input!");
        shmdt(shares);
        exit(1);
    }

    // Check to make sure the buffer is empty, then fill it
    if(shares->bufFlag[0] == empty)
    {
        fgets(shares->buf0, 20, fp);
        shares->bufFlag[0] = full;
    }

    // Close file and detach shared memory.
    fclose(fp);

    shmdt(shares);
    printf("Shared memory has been detached.\n");
    return 0;
}
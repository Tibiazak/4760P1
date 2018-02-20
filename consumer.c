//
// Created by fof_z on 2/19/2018.
//

#include <stdio.h>
#include <unistd.h>


int main(void)
{
    printf("Hello, I'm consumer process %d, and my parent is %d", getpid(), getppid());
    return 0;
}
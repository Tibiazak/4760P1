//
// Created by fof_z on 2/19/2018.
//

#include "producer.h"

int main(void)
{
    printf("Hello, I'm producer process %d and my parent is process %d\n", getpid(), getppid());
    return 0;
}
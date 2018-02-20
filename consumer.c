//
// Created by fof_z on 2/19/2018.
//

int main(void)
{
    printf("Hello, I'm consumer process %d, and my parent is %d", getpid(), getppid());
    return 0;
}
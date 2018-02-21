#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "flags.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define BILLION 1000000000L
#define TIMER_MSG "Received Timer interrupt \n"
#define FLAGKEY 92195
#define FLAGKEYSTR "92195"
#define TURNKEY 92295
#define TURNKEYSTR "92295"
#define BUFSZ 20

// A function from the setperiodic code, not entirely sure how it works
static void interrupt(int signo, siginfo_t *info, void *context)
{
    int errsave;

    errsave = errno;
    write(STDOUT_FILENO, TIMER_MSG, sizeof(TIMER_MSG) - 1);
    errno = errsave;
}

// A function from the setperiodic code, not entirely sure how it works
static int setinterrupt()
{
    struct sigaction act;

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = interrupt;
    if ((sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGALRM, &act, NULL) == -1))
    {
        return 1;
    }
    return 0;
}

// Eventually: A function to send a ctrl+c signal to this process after 100 seconds
// Currently: a function that prints to the screen every 2 seconds indefinitely
static int setperiodic(double sec)
{
    timer_t timerid;
    struct itimerspec value;

    if (timer_create(CLOCK_REALTIME, NULL, &timerid) == -1)
    {
        return -1;
    }
    value.it_interval.tv_sec = (long)sec;
    value.it_interval.tv_nsec = (sec - value.it_interval.tv_sec)*BILLION;
    if (value.it_interval.tv_nsec >= BILLION)
    {
        value.it_interval.tv_sec++;
        value.it_interval.tv_nsec -= BILLION;
    }
    value.it_value = value.it_interval;
    return timer_settime(timerid, 0, &value, NULL);
}

// Fork and exec a producer with required keys/variables
int makeProducer(void)
{
    char *argarray[]={"./producer", FLAGKEYSTR, TURNKEYSTR, NULL};
    int pid = fork();
    if (pid == 0)
    {
        execvp(argarray[0], argarray);
    }
    return(pid);
}

// Fork and exec a consumer with required keys/variables
int makeConsumer(void)
{
    char *argarray[]={"./consumer", FLAGKEYSTR, TURNKEYSTR, NULL};
    int pid = fork();
    if (pid == 0)
    {
        execvp(argarray[0], argarray);
    }
    return(pid);
}

// Detach and deallocate all shared memory
int freeshm(int id1, int id2, struct flags *ptr1, int *ptr2){
    shmdt((void *) ptr1);
    shmctl(id1, IPC_RMID, NULL);
    shmdt((void *) ptr2);
    shmctl(id2, IPC_RMID, NULL);
}

int main(void) {
//    if (setinterrupt() == -1)
//    {
//        perror("Failed to setup SIGALRM handler");
//        return 1;
//    }
//    if (setperiodic(2.0) == -1)
//    {
//        perror("Failed to setup periodic interrupt");
//        return 1;
//    }
//    for ( ; ; )
//    {
//        pause();
//    }


    // create shared memory flag
    int flagid, turnid;
    struct flags *flag;
    int * turn;
    int status = 0;
    int i = 0;
    pid_t pid, wpid;

    // allocate shared memory for the flag
    flagid = shmget(FLAGKEY, sizeof(struct flags), 0777 | IPC_CREAT);
    if(flagid == -1)
    {
        perror("flag shmget");
        exit(1);
    }
    // attach shared memory to the flag variable
    flag = (struct flags *)(shmat(flagid, 0, 0));
    if((int) flag == -1)
    {
        perror("flag shmat");
        exit(1);
    }
    flag->status = IDLE;

    // allocate shared memory for the turn
    turnid = shmget(TURNKEY, sizeof(int), 0777 | IPC_CREAT);
    if(turnid == -1)
    {
        perror("turn shmget");
        exit(1);
    }

    // attach shared memory to the turn variable
    turn = (int *)(shmat(turnid, 0, 0));
    if((int) turn == -1)
    {
        perror("turn shmat");
        exit(1);
    }

    *turn = 1;

    makeProducer();
    *turn += 1;

    makeConsumer();
    *turn += 1;

    while ((wpid = wait(&status)) > 0); // waits for all processes to finish

    printf("All children terminated.\n");
    printf("Number of processes is: %d\n", *turn);
    freeshm(flagid, turnid, flag, turn); // free shared memory
    printf("All shared memory detached and freed\n");

    return(0);
}

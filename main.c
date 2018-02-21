#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "share.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define BILLION 1000000000L
#define TIMER_MSG "Received Timer interrupt \n"
#define SHAREKEY 92195
#define SHAREKEYSTR "92195"
#define MAX_PROCS 17
#define DEFAULT_PROCS 10

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
    char *argarray[]={"./producer", SHAREKEYSTR, NULL};
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
    char *argarray[]={"./consumer", SHAREKEYSTR, NULL};
    int pid = fork();
    if (pid == 0)
    {
        execvp(argarray[0], argarray);
    }
    return(pid);
}

// Detach and deallocate all shared memory
int freeshm(int shareid, struct share *shares){
    shmdt((void *) shares);
    shmctl(shareid, IPC_RMID, NULL);
    return(0);
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
    int shareid;
    struct share *shares;
    int status = 0;
    int i = 0;
    pid_t pid, wpid;

    shareid = shmget(SHAREKEY, sizeof(struct share), 0777 | IPC_CREAT);
    if(shareid == -1)
    {
        perror("Master shmget");
        exit(1);
    }

    shares = (struct share *) (shmat(shareid, 0, 0));
    if((int) shares == -1)
    {
        perror("Master shmat");
        exit(1);
    }

    enum state * s = &(shares->bufFlag[0]);
    for(i = 0; i < 5; i++)
    {
        *s = empty;
        s += 1;
    }

    makeProducer();

    makeConsumer();

    while ((wpid = wait(&status)) > 0); // waits for all processes to finish

    printf("All children terminated.\n");
    freeshm(shareid, shares); // free shared memory
    printf("All shared memory detached and freed\n");

    return(0);
}

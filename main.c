/* Joshua Bearden
 * 2/21/18
 * Operating Systems Project 2
 *
 * A program to fork a producer and a number of consumer child processes,
 * where the producer fills 5 buffers with text from an input file
 * and the consumers empty the buffers and add to log files.
 *
 * This project needs to use shared memory and Peterson's Multiple Process Solution
 * to the critical section problem.
 *
 * This project is incomplete - currently it creates (and empties) all of the shared
 * memory needed (5 buffers, flag variables). The producer grabs one line from the
 * input file, fills a buffer with it and sets the appropriate flag. The consumer
 * grabs that file and prints it out to the screen.
 *
 * Much of the code for the 100 second timer is in there, however the signal handler is not
 * working yet. Peterson's solution has also not been implemented. The actual work of
 * filling and emptying buffers, while not complete, is trivial to implement once the signal
 * handler and peterson's solution are implemented.
 */

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

// A function from the setperiodic code, catches the interrupt and prints to screen
static void interrupt(int signo, siginfo_t *info, void *context)
{
    int errsave;

    errsave = errno;
    write(STDOUT_FILENO, TIMER_MSG, sizeof(TIMER_MSG) - 1);
    errno = errsave;
}

// A function from the setperiodic code, it sets up the interrupt handler
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

// A function that sets up a timer to go off after a specified number of seconds
// The timer only goes off once
static int setperiodic(double sec)
{
    timer_t timerid;
    struct itimerspec value;

    if (timer_create(CLOCK_REALTIME, NULL, &timerid) == -1)
    {
        return -1;
    }
    value.it_value.tv_sec = (long)sec;
    value.it_value.tv_nsec = 0;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = 0;
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

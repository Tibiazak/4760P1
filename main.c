#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#define BILLION 1000000000L
#define TIMER_MSG "Received Timer interrupt \n"

static void interrupt(int signo, siginfo_t *info, void *context)
{
    int errsave;

    errsave = errno;
    write(STDOUT_FILENO, TIMER_MSG, sizeof(TIMER_MSG) - 1);
    errno = errsave;
}

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

int makeProducer(void)
{
    char *argarray[]={"./producer", NULL};
    int pid = fork();
    if (pid == 0)
    {
        execvp(argarray[0], argarray);
    }
    return(pid);
}

int makeConsumer(void)
{
    char *argarray[]={"./consumer", NULL};
    int pid = fork();
    if (pid == 0)
    {
        execvp(argarray[0], argarray);
    }
    return(pid);
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
    int status = 0;
    int i = 0;
    pid_t pid, wpid;
    printf("Creating producer\n");
    pid = makeProducer();
    printf("Producer created, pid: %d", pid);

    for(i = 0; i < 3; i++)
    {
        printf("Creating consumer\n");
        pid = makeConsumer();
        printf("Consumer created:, pid: %d", pid);
    }
    while ((wpid = wait(&status)) > 0);
    printf("All children finished\n");
    return(0);
}

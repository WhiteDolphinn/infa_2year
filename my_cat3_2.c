#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

static pid_t parent_pid;
static pid_t child_pid;

siginfo_t info;
char buf[1] = {};
struct timespec ttd = {0, 1000};

void bit_receive(int sig, siginfo_t* info, void* zhopabobra);
void bit_send();
void end_prog();

void end_prog() {
    exit(0);
}

void bit_receive(int sig, siginfo_t* info, void* zhopabobra)
{
    char value = info->si_value.sival_int;

    //printf("prrec: %c (%d)\n", value, (int)value);

    write(1, &value, sizeof(char));
}

void bit_send()
{
    union sigval sigv;
    sigv.sival_int = *buf;

    //printf("prsend: %c (%d)\n", (char)(*buf), *buf);

    int val = 0;
    while((val = sigqueue(child_pid, SIGRTMIN, sigv)) < 0)
    {
        //printf("prsend: val = %d\n", val);
        perror("Fall");
        usleep(1000);
    }

}

int main()
{
    pid_t pid = fork();

    if (pid == 0)
    {
        parent_pid = getppid();

       // sigqueue(parent_pid, SIGUSR1, buf[0]);

        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGRTMIN);
        sigdelset(&mask, SIGINT);
        sigdelset(&mask, SIGRTMIN + 1);
        sigdelset(&mask, SIGKILL);
        sigdelset(&mask, SIGSTOP);

        struct sigaction sigact = {};
        sigact.sa_flags = SA_SIGINFO;
        sigact.sa_mask = mask;
        sigact.sa_sigaction = bit_receive;

        sigaction(SIGRTMIN, &sigact, NULL);

        while(1)
        {
            sigtimedwait(&mask, &sigact, &ttd);
        }

        return 0;
    }

    child_pid = pid;
    //usleep(10000);

    while(read(0, buf, sizeof(char)))
    {
        bit_send();
    }


    return 0;
}

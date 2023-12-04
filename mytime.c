#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char* const* argv)
{
    struct timespec time_start = {};
    struct timespec time_end = {};

    clock_gettime(CLOCK_BOOTTIME, &time_start);

    pid_t pid = fork();

    if(pid == 0)
        execvp(argv[1], argv + 1);

    wait(NULL);
    clock_gettime(CLOCK_BOOTTIME, &time_end);
    printf("Time_end_sec = %ld\n", time_end.tv_sec);
    printf("Time_start_sec= %ld\n", time_start.tv_sec);
    printf("Time_end_nsec = %ld\n", time_end.tv_nsec);
    printf("Time_start_nsec= %ld\n", time_start.tv_nsec);
    //printf("Time_sec = %ld\n", time_end.tv_sec - time_start.tv_sec);

    long time_sec = time_end.tv_sec - time_start.tv_sec;
    long time_nsec = 0;
    if(time_end.tv_nsec > time_start.tv_nsec)
        time_nsec = time_end.tv_nsec - time_start.tv_nsec;
    else
    {
        time_nsec = (time_start.tv_nsec - time_end.tv_nsec);
        time_sec--;
    }
    printf();
    printf("Time = %.3ld\n", time_sec + (time_nsec/1000000000));
    return 0;
}

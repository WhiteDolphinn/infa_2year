#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>

int main()
{
    int fds[2] = {};
    pipe(fds);

    pid_t pid = fork();

    if(pid == 0)
    {
        return 0;
    }

    fcntl(fds[1], F_SETFL, O_NONBLOCK);

    char ch = 'p';
    int sch = 0;

    while(write(fds[1], &ch, 1) > 0)
        sch++;

    printf("sch = %d\n", sch);

    return 0;
}

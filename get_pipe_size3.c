#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int size = 0;
int fds[2] = {};

void alarmed();

int main()
{

    pipe(fds);

    char ch = 'h';

    alarm(2);
    signal(SIGALRM, alarmed);

    for(int i = 0; i < 1000000; i++)
    {
        write(fds[1], &ch, 1);
        size++;
    }

    printf("%d\n", size);
    close(fds[0]);
    close(fds[1]);

    return 0;
}

void alarmed()
{
    printf("%d\n", size);
    close(fds[0]);
    close(fds[1]);
    exit(0);
}

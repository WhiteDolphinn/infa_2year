#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define T 10000

int main(int argc, const char* argv[])
{
    for(int i = 1; i < argc; i++)
    {
        pid_t pid = fork();
        if(pid == 0)
        {
            /*unsigned val = 0;
            sscanf(argv[i], "%u", &val);*/
            unsigned val = atoi(argv[i]);
            usleep(val * T);
            printf("%u ", val);
            return 0;
        }
    }

    for(int i = 1; i < argc; i++)
        wait(NULL);

   // usleep(10000000);
    printf("\n");
    return 0;
}

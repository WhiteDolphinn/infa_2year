#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define N 5
#define SIZE 256

struct msgbuf{
    long mtype;
    char* mtext;
};

void runner_start(int id, int i);
void judge(int id);

int main(int argc, const char** argv)
{
    int key = ftok(argv[0], 'x');
    int id = msgget(key, IPC_CREAT | 0777);

    pid_t pid1 = fork();

    if(pid1 == 0)
    {
        judge(id);
        return 0;
    }

    for(int i = 1; i <= N; i++)
    {
        pid_t pid = fork();

        if(pid == 0)
        {
            runner_start(id, i);
            return 0;
        }
    }

    for(int i = 0; i < N; i++)
        wait(NULL);

    msgctl(id, IPC_RMID, 0);
    return 0;
}

void runner_start(int id, int i)
{
        struct msgbuf buf = {};
        buf.mtype = N+1;
        printf( "YA PRISHEL! NUM:%d\n", i);

        msgsnd(id, (const void*)&buf, SIZE, 0);


        msgrcv(id, &buf, SIZE, i, 0);

        buf.mtype = i + 1;

        msgsnd(id, (const void*)&buf, SIZE, 0);
}

void judge(int id)
{
    for(int i = 1; i <= N; i++)
    {
        struct msgbuf buf = {};
        if(msgrcv(id, &buf, SIZE, N+1, 0) < 0)
            perror("sofhvjsplvnsjpsvpsfvnsf");
        printf("JUDGE POLUCHIL MSG OT RUNNER NUM %d\n", i);
    }

    struct msgbuf buf_run = {};
    buf_run.mtype = 1;
    msgsnd(id, (const void*)(&buf_run), SIZE, 0);

    msgrcv(id, (&buf_run), SIZE, N, 0);
    printf("END RUN\n");
}

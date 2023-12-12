#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int tr = 0;

void semb_init(struct sembuf* semb, unsigned short sem_num, short sem_op, short sem_flg);

int main()
{
    int fds[2] = {};
    pipe(fds);

    int id = semget(1, 1, IPC_CREAT | 0777);
    semctl(id, tr, SETVAL, 0);

    pid_t pid = fork();

    if(pid == 0)
    {
        struct sembuf semb = {};

        char ch = 'h';
        for(int i = 0; i < 1000000; i++)
        {
            semb_init(&semb, tr, 0, 0);
            semop(id, &semb, 1);
            write(fds[1], &ch, 1);
            semb_init(&semb, tr, 1, 0);
            semop(id, &semb, 1);
        }

        return 0;
    }

    int sem1 = 0;
    //int sem2 = 0;
    int size = 0;

    struct sembuf semb = {};
    sleep(2);

    for(int i = 0; i < 1000000; i++)
    {
        usleep(1000);
        sem1 = semctl(id, tr, GETVAL);

        if(sem1 == 1)
        {
            size++;
            semb_init(&semb, tr, -1, 0);
            semop(id, &semb, 1);
            printf("%d\n", size);
        }
        else
            break;
    }

    printf("%d\n", size);

    close(fds[0]);

    return 0;
}


void semb_init(struct sembuf* semb, unsigned short sem_num, short sem_op, short sem_flg)
{
    semb->sem_num = sem_num;
    semb->sem_op = sem_op;
    semb->sem_flg = sem_flg;
    //semb->sem_flg = SEM_UNDO;
}

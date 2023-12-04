#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <errno.h>

#define SEMOP         semop(id, &semb, 1)

void semb_init(struct sembuf* semb, unsigned short sem_num, short sem_op, short sem_flg);

int main()
{
    int id = semget(IPC_CREAT, 1, 0);
    if(id < 0)
        perror("sobvsdobv");

    struct sembuf semb = {};

    semctl(id, 0, SETVAL, 0);

    semb_init(&semb, 0, 1, 0);
    if(SEMOP < 0)
        perror("pndfovdf");


    semctl(id, 0, IPC_RMID);
    return 0;
}

void semb_init(struct sembuf* semb, unsigned short sem_num, short sem_op, short sem_flg)
{
    semb->sem_num = sem_num;
    semb->sem_op = sem_op;
    semb->sem_flg = sem_flg;
}

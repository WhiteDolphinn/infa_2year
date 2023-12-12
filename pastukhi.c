#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <time.h>

#define SEMOP   semop(id, &semb, 1)
#define SEM2OP  semop(id, semb2, 2)

int N = 25;
int M = 20;

#define NUM_OF_SEMS 4
enum sem{
    cap = 0,
    br_free = 1,
    queue1 = 2,
    queue2 = 3,
};

void pass1(int id, int pass_number);
void pass2(int id, int pass_number);
void semb_init(struct sembuf* semb, unsigned short sem_num, short sem_op, short sem_flg);

int main(int argc, const char** argv)
{
    if(argc == 3)
    {
        N = argv[1][0] - '0';
        M = argv[2][0] - '0';

    }

    int id = semget(IPC_PRIVATE, NUM_OF_SEMS, IPC_CREAT | 0777);
    if(id < 0)
        perror("SEMGET_ERROR");

    semctl(id, cap, SETVAL, 0);
    semctl(id, br_free, SETVAL, 0);
    semctl(id, queue1, SETVAL, 1);
    semctl(id, queue2, SETVAL, 1);

    for(int i = 0; i < N; i++)
    {
        usleep(100000*i);
        pid_t pid = fork();

        if(pid == 0)
        {
            pass1(id, i);
            return 0;
        }
    }

    for(int i = 0; i < M; i++)
    {
        usleep(100000*i);
        pid_t pid = fork();

        if(pid == 0)
        {
            pass2(id, i);
            return 0;
        }
    }

    for(int i = 0; i < N + M; i++)
        wait(NULL);

    semctl(id, 0, IPC_RMID);


    return 0;
}

void pass1(int id, int pass_number)
{
    struct sembuf semb = {};

    semb_init(&semb, queue1, -(pass_number+1), 0);
    SEMOP;
    semb_init(&semb, queue1, (pass_number+1), 0);
    SEMOP; //ждём очередь
    printf("Пастух-> н.%d  дождался очереди\n", pass_number);

    if(semctl(id, cap, GETVAL) == 0)
    {
        semb_init(&semb, br_free, 0, 0);
        SEMOP; //ждём, когда мост освободится
        printf("Пастух-> н.%d дождался освобождения моста, чтобы пойти с шапкой на другой конец\n", pass_number);
        usleep(100000);

        semb_init(&semb, cap, 1, 0);
        SEMOP; //оставляем шапку
        printf("Пастух-> н.%d оставил шапку\n", pass_number);
        usleep(100000);

        semb_init(&semb, br_free, 0, 0);
        SEMOP; //ждём, когда мост освободится
        printf("Пастух-> н.%d дождался освобождения моста, чтобы пойти со стадом.\n", pass_number);
    }
    else
    {
        printf("Пастух-> н.%d присоединился к стаду.\n", pass_number);
    }

    semb_init(&semb, queue1, 1, 0);
    SEMOP; //увеличиваем очередь

    semb_init(&semb, br_free, 1, 0);
    SEMOP; //занимаем мост
    printf("Пастух-> н.%d. Стадо пошло.\n", pass_number);
    usleep(300000*(semctl(id, queue1, GETVAL) - pass_number));

    semb_init(&semb, br_free, -1, 0);
    SEMOP;
    printf("Пастух-> н.%d освободил мост. Стадо прошло.\n", pass_number);

    semb_init(&semb, cap, -1, 0);
    SEMOP;
    printf("Пастух-> н.%d забрал шапку\n", pass_number);
}

void pass2(int id, int pass_number)
{
    struct sembuf semb = {};

    semb_init(&semb, queue2, -(pass_number+1), 0);
    SEMOP;
    semb_init(&semb, queue2, (pass_number+1), 0);
    SEMOP; //ждём очередь
    printf("Пастух<- н.%d  дождался очереди\n", pass_number);

    semb_init(&semb, cap, 0, 0);
    SEMOP; //ждём остутствие шапки
    printf("Пастух<- н.%d  дождался отсутствия шапки\n", pass_number);

    semb_init(&semb, br_free, 1, 0);
    SEMOP; //занимаем мост
    printf("Пастух<- н.%d  пошёл по мосту со стадом\n", pass_number);
    semb_init(&semb, queue2, 1, 0);
    SEMOP; //увеличиваем очередь
    usleep(300000);

    semb_init(&semb, br_free, -1, 0);
    SEMOP; //освобождаем мост
    printf("Пастух<- н.%d освободил мост\n", pass_number);

}

void semb_init(struct sembuf* semb, unsigned short sem_num, short sem_op, short sem_flg)
{
    semb->sem_num = sem_num;
    semb->sem_op = sem_op;
    semb->sem_flg = sem_flg;
    //semb->sem_flg = SEM_UNDO;
}

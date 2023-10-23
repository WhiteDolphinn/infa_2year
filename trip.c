#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

#define N 50
#define M 20
#define K 5
#define NUM_OF_TRIPS 3
// 50 20 5 10
#define NUM_OF_SEMS 6
enum sem{
    tr = 0,
    bt = 1,
    trap_beach = 2,
    trap_boat = 3,
    tickets = 4,
    trip_start = 5,
};

#define SEMOP         semop(id, &semb, 1)


void pass(int id, int pass_number);
void boat(int id);

void semb_init(struct sembuf* semb, unsigned short sem_num, short sem_op, short sem_flg);

int main()
{
    int id = semget(1, NUM_OF_SEMS, IPC_CREAT | 0777);
    if(id < 0)
        perror("SEMSET_ERROR");

    //printf("id = %d\n", id);

    semctl(id, tr, SETVAL, K);
    semctl(id, bt, SETVAL, M);
    semctl(id, trap_beach, SETVAL, 1);
    semctl(id, trap_boat, SETVAL, 1);
    semctl(id, tickets, SETVAL, 0);
    semctl(id, trip_start, SETVAL, 1);


    for(int i = 0; i < N; i++)
    {
        pid_t pid = fork();

        if(pid == 0)
        {
            pass(id, i);
            return 0;
        }
    }

    boat(id);

    for(int i = 0; i < NUM_OF_SEMS-1; i++) //чтобы пассажиры не ждали пароход, которого уже не будет
        semctl(id, i, SETVAL, 0);

    for(int i = 0; i < NUM_OF_SEMS-1; i++)
        semctl(id, i, IPC_RMID);

    return 0;
}


void boat(int id)
{
    for(int i = 0; i < NUM_OF_TRIPS; i++)
    {
        struct sembuf semb = {};

        semb_init(&semb, trap_beach, -1, 0);
        if(SEMOP < 0) perror("bebra");
        semb_init(&semb, trap_boat, -1, 0);
        if(SEMOP < 0) perror("bebra"); // опускаем трап
        printf("ТРАП ОПУЩЕН. \n");

        //sleep(2); //стоим
        semb_init(&semb, bt, -M, 0);
        SEMOP; // ждём, когда все сойдут с корабля
        semb_init(&semb, bt, M, 0);
        SEMOP;


        printf("НАЧАЛО ПРОДАЖИ БИЛЕТОВ. ПОЕЗДКА НОМЕР:%d\n", i);
        semctl(id, tickets, SETVAL, M); // запускаем продажу билетов

        /*semb_init(&semb, tickets, 0, 0);
        if(SEMOP < 0) perror("zhopa"); // ждём когда продадут все билеты*/

        /*semb_init(&semb, trap_beach, -1, 0);
        if(SEMOP < 0) perror("bebra");
        semb_init(&semb, trap_boat, -1, 0);
        if(SEMOP < 0) perror("bebra"); // опускаем трап
        printf("ТРАП ОПУЩЕН. \n");*/


        semb_init(&semb, bt, 0, 0);
        if(SEMOP < 0) perror("bebra"); // ждём заполнения всех мест на корабле

        semb_init(&semb, tr, -K, 0);
        SEMOP; // ждём когда все уйдут с трапа
        semb_init(&semb, tr, K, 0);
        SEMOP;

        semb_init(&semb, trap_beach, 1, 0);
        SEMOP;
        semb_init(&semb, trap_boat, 1, 0);
        SEMOP; // поднимаем трап
        printf("ТРАП ПОДНЯТ. \n");

        semb_init(&semb, trip_start, -1, 0);
        SEMOP; // отплытие
        printf("ОТПЛЫТИЕ. \n");

        sleep(2); //катаемся

        semb_init(&semb, trip_start, 1, 0);
        SEMOP; // прибытие
        printf("ПРИБЫТИЕ. \n");

    }
    printf("КОНЕЦ РЕЙСОВ\n");
}

void pass(int id, int pass_number)
{
    for(int i = 0; i < NUM_OF_TRIPS; i++)
    {

        struct sembuf semb = {tickets, -1, 0};
        SEMOP; // ждём свободные билеты
        printf("Пассажир н.%d купил билет\n", pass_number);

        semb_init(&semb, trap_beach, 0, 0);
        SEMOP; // ждём опускания трапа

        semb_init(&semb, tr, -1, 0);
        SEMOP; // ждём свободные места на трапе
        printf("Пассажир н.%d зашёл на трап.\n", pass_number);

        semb_init(&semb, trap_boat, 0, 0);
        SEMOP; // убеждаемся, что второй конец трапа открыт

        semb_init(&semb, bt, -1, 0);
        SEMOP; // заходим на борт
        printf("Пассажир н.%d зашёл на борт\n", pass_number);

        semb_init(&semb, tr, 1, 0);
        SEMOP; // зашли на борт -> на трапе стало на 1 свободное место больше

        semb_init(&semb, trip_start, 0, 0);
        SEMOP; // ждём отправления и катаемся

        semb_init(&semb, trap_boat, 0, 0);
        SEMOP; // убеждаемся, что можно попасть на трап

        semb_init(&semb, tr, -1, 0);
        SEMOP; // ждём свободные места на трапе
        printf("Пассажир н.%d сошёл с борта.\n", pass_number);

        semb_init(&semb, bt, 1, 0);
        SEMOP; // зашли на трап -> на борту стало на 1 свободное место больше

        semb_init(&semb, trap_beach, 0, 0);
        SEMOP; // убеждаемся, что открыт второй конец трапа
        printf("Пассажир н.%d сошёл с трапа.\n", pass_number);

        semb_init(&semb, tr, 1, 0);
        SEMOP; // сошли с трапа

    }
}

void semb_init(struct sembuf* semb, unsigned short sem_num, short sem_op, short sem_flg)
{
    semb->sem_num = sem_num;
    semb->sem_op = sem_op;
    semb->sem_flg = sem_flg;
    //semb->sem_flg = SEM_UNDO;
}

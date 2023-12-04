#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>

enum sem{
    first_buf_empty = 0,
    second_buf_empty = 1,
    first_buf_full = 2,
    second_buf_full = 3,
};

struct rw_buf{
    char buf1[4096];
    char buf2[4096];

    int buf_size_1;
    int buf_size_2;
    //int mode; // 1 - read-1, write-2 || 2 - read-2, write-1
};

#define SEMOP         semop(sem_id, &semb, 1)

void semb_init(struct sembuf* semb, unsigned short sem_num, short sem_op, short sem_flg);
int read_from_fd_to_buf(int fd, void* buf, size_t count);
ssize_t my_write(int fd, void* buf, size_t count);

void reader(struct rw_buf* buf, int argc, int sem_id, const char** argv);
void writer(struct rw_buf* buf, int argc, int sem_id);

int main(int argc, const char** argv)
{
    int shm_id = shmget(IPC_PRIVATE, sizeof(struct rw_buf), IPC_CREAT | 0777);
    if(shm_id == -1)
        perror("mem_id");

    struct rw_buf* buf = shmat(shm_id, NULL, 0);
    if((void*)buf == (void*)-1)
        perror("buf");

    int sem_id = semget(IPC_PRIVATE, 4, IPC_CREAT | 0777);
    if(sem_id == -1)
        perror("sem_id");

    semctl(sem_id, first_buf_empty, SETVAL, 0);
    semctl(sem_id, second_buf_empty, SETVAL, 0);
    semctl(sem_id, first_buf_full, SETVAL, 1);
    semctl(sem_id, second_buf_full, SETVAL, 1);

    pid_t pid1 = fork();

    if(pid1 == 0)
    {
        writer(buf, argc, sem_id);
    }

    pid_t pid2 = fork();

    if(pid2 == 0)
    {
        reader(buf, argc, sem_id, argv);
    }

    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}

void writer(struct rw_buf* buf, int argc, int sem_id)
{
    struct sembuf semb = {};

    for(int i = 1; i/2 < argc; i++)
    {
        semb_init(&semb, first_buf_full, 0, 0);
        SEMOP;
        semb_init(&semb, first_buf_full, 1, 0);
        SEMOP;

        my_write(1, buf->buf1, buf->buf_size_1);

        semb_init(&semb, first_buf_empty, -1, 0);
        SEMOP;


        semb_init(&semb, second_buf_full, 0, 0);
        SEMOP;
        semb_init(&semb, second_buf_full, 1, 0);
        SEMOP;

        my_write(1, buf->buf2, buf->buf_size_2);

        semb_init(&semb, second_buf_empty, -1, 0);
        SEMOP;
    }
}

void reader(struct rw_buf* buf, int argc, int sem_id, const char** argv)
{
    struct sembuf semb = {};

    for(int i = 1; i/2 < argc; i++)
    {
        semb_init(&semb, first_buf_empty, 0, 0);
        SEMOP;
        semb_init(&semb, first_buf_empty, 1, 0);
        SEMOP;

        int buf_size = 0;
        int fd = open(argv[i], O_RDONLY);

        while((buf_size = read_from_fd_to_buf(fd, buf->buf1, 4096)) > 0)
            continue;

        if(buf_size == -1)
            printf("Error in buf_size.\n");

        buf->buf_size_1 = buf_size;

        semb_init(&semb, first_buf_full, -1, 0);
        SEMOP;


        semb_init(&semb, second_buf_empty, 0, 0);
        SEMOP;
        semb_init(&semb, second_buf_empty, 1, 0);
        SEMOP;

        buf_size = 0;
        fd = open(argv[i+1], O_RDONLY);

        while((buf_size = read_from_fd_to_buf(fd, buf->buf2, 4096)) > 0)
            continue;

        if(buf_size == -1)
            printf("Error in buf_size.\n");

        buf->buf_size_2 = buf_size;

        semb_init(&semb, second_buf_full, -1, 0);
        SEMOP;
    }
}

void semb_init(struct sembuf* semb, unsigned short sem_num, short sem_op, short sem_flg)
{
    semb->sem_num = sem_num;
    semb->sem_op = sem_op;
    semb->sem_flg = sem_flg;
    //semb->sem_flg = SEM_UNDO;
}


int read_from_fd_to_buf(int fd, void* buf, size_t count)
{
   //char buffer[count] = {};
    int buf_size = 0;

    while((buf_size += read(fd, buf, count)))
    {
        if(buf_size < 0)
            return -1;

        //my_write(1, buf, buf_size);

    }

    return buf_size;
}

ssize_t my_write(int fd, void* buf, size_t count)
{
    size_t i = 0;
    while(i < count)
    {
        ssize_t write_val = write(fd, buf + i, count - i);

        if(write_val < 0)
            return write_val;

        i += write_val;
    }

    return i;
}

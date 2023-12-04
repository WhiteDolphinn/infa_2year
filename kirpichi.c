#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include <sys/ipc.h>
//#include <sys/msg.h>
//#include <sys/sem.h>
#include <pthread.h>

#define N 10
#define M 20

void monitor_init(struct monitor* mon);
void monitor_finit(struct monitor* mon);
void monitor_put_brick_parent(struct monitor* mon);
void monitor_put_brick_child(struct monitor* mon);
void child(struct monitor* mon);
void parent(struct monitor* mon);

struct monitor{
    pthread_mutex_t n;
    pthread_condition_t child_wait_work, parent_wait_work;

    void (*monitor_init)(struct monitor*);
    void (*monitor_finit)(struct monitor*);
    void (*monitor_put_brick_child)(struct monitor*);
    void (*monitor_put_brick_parent)(struct monitor*);
};

void monitor_init(struct monitor* mon)
{
    pthread_mutex_init(&mon->n, NULL);
    pthread_mutex_lock(&mon->n);
    mon->monitor_init = monitor_init;
    mon->monitor_finit = monitor_finit;
    mon->monitor_put_brick_child = motitor_put_brick_child;
    mon->monitor_put_brick_parent = motitor_put_brick_parent;
    mon->monitor_child = monitor_child;
    mon->monitor_parent = monitor_parent;
    pthread_mutex_unlock(&mon.n);
}


void monitor_finit(struct monitor* mon)
{
    pthread_mutex_lock(&mon->n, NULL);
    mon->init = NULL;
    mon->finit = NULL;
    mon->put_brick = NULL;
    pthread_mutex_destroy(&mon->n);
}

void monitor_put_brick_parent(struct monitor* mon)
{

}

void monitor_put_brick_child(struct monitor* mon)
{

}

void child(struct monitor* mon)
{
    for(int i = 1; i < M; )
    {
        for(int j = 0; i < N; j++)
        {
            monitor_put_brick_child(mon);
            pthread_mutex_signal(parent_wait_work);
        }

        i += 2;
    }
}

void parent(struct monitor* mon)
{
    for(int i = 0; i < M; )
    {
        for(int j = 0; i < N; j++)
        {
            monitor_put_brick_parent(mon);
            pthread_mutex_signal(child_wait_work);
        }

        i += 2;
    }
}

int main()
{
    struct monitor mon;
    monitor_init(&mon);

    pthread_t tid1;
    pthread_create(&tid1, NULL, child, NULL);

    pthread_t tid2;
    pthread_create(&tid2, NULL, parent, NULL);


    monitor_finit(&mon);
    return 0;
}


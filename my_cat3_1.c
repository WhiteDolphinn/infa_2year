#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

static int byte;
static int bit_in_byte = 7;
static pid_t parent_pid;
static char byte_s;
static int bit_in_byte_s = -1;
static pid_t child_pid;

void bit_receive(int num_sign);
void bit_send();
void end_prog();

void end_prog() {
    exit(0);
}

void bit_receive(int num_sign) {
    int bit = 0;

    if (num_sign == SIGUSR1) {
        bit = 0;
    }

    if(num_sign == SIGUSR2)
        bit = 1;

    bit = bit << bit_in_byte;
    byte += bit;
    bit_in_byte--;

    //printf("printf: byte = %c (%d)\t bit_in_byte = %d\t bit = %d\n", byte, (int)byte, bit_in_byte, bit);

    if (bit_in_byte == -1) {
        char buf[1];
        buf[0] = byte;
        write(1, buf, 1);
        byte = 0;
        bit_in_byte = 7;
    }


    kill(parent_pid, SIGUSR1);
}

void bit_send()
{
    if (bit_in_byte_s == -1)
    {
        byte_s = 0;
        char buf[1];
        if (read(0, buf, 1) == 0)
        {
            kill(child_pid, SIGINT);
            exit(0);
        }
        byte_s = buf[0];
        bit_in_byte_s = 7;
    }

    //printf("printf: buf = %c (%d)\n", byte_s, (int)byte_s);
    //printf("printf: byte_s & (1 << bit_in_byte_s)) = %d\t bit_in_byte_s = %d\n", byte_s & (1 << bit_in_byte_s), bit_in_byte_s);

    bit_in_byte_s -= 1;

    if (byte_s & (1 << (bit_in_byte_s + 1)))
    {
        //printf("sig: SIGUSR2 (1)\t bit_in_byte_s = %d\n", bit_in_byte_s);
        kill(child_pid, SIGUSR2);
    }
    else
    {
        //printf("sig: SIGUSR1 (0)\t bit_in_byte_s = %d\n", bit_in_byte_s);
        kill(child_pid, SIGUSR1);
    }


}

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        signal(SIGUSR1, bit_receive);
        signal(SIGUSR2, bit_receive);
        signal(SIGINT, end_prog);
        parent_pid = getppid();

        sleep(2);
        //pause();

        kill(parent_pid, SIGUSR1);

        while (1)
            pause();
    }

    signal(SIGUSR1, bit_send);
    signal(SIGUSR2, bit_send);
    signal(SIGINT, end_prog);
    child_pid = pid;

    //kill(child_pid, SIGUSR1);
    while (1)
        pause();

}

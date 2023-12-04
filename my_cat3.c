#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

char bit = 0;
char byte = 0;
char cur_bit_in_byte = 0;

ssize_t my_write(int fd, void* buf, size_t count);

void end_func()
{
    exit(0);
}

void bit_receive()
{
    byte &= bit;
    byte = byte << 1;


    if(cur_bit_in_byte == 8)
    {
        cur_bit_in_byte = 0;
        kill(getppid(), SIGUSR2)
    }
}

int main()
{
    pid_t pid = fork();
    signal(SIGINT, end_func);

    if(pid == 0)
    {
        //signal(SIGUSR1, bit_receive); //0
        signal(SIGUSR2, bit_receive); //1



        return 0;
    }


    for(int i = 0; i < strlen(buf); i++)
        for(int j = 0; j < 8; j++)
        {
            bit = (buf[i] >> (7-j)) & 1;

            /*if(bit == 0)
                kill(0, SIGUSR1);
            else
                kill(0, SIGUSR2);*/
            kill(0, SIGUSR1);

            pause();
        }

    return 0;
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

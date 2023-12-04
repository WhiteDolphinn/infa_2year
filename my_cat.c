#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 8192

ssize_t my_write(int fd, void* buf, size_t count);
int write_from_fd(int fd, void* buf, size_t count);

int main(int argc, const char** argv)
{
     char buffer[SIZE] = {};

    if(argc == 1)
    {
        return write_from_fd(0, buffer, SIZE);
    }

    for(int i = 1; i < argc; i++)
    {
        int fd = open(argv[i], O_RDONLY);
        write_from_fd(fd, buffer, SIZE);
        close(fd);
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

int write_from_fd(int fd, void* buf, size_t count)
{
    //char buffer[count] = {};
    int buf_size = 0;

    while((buf_size = read(fd, buf, count)))
    {
        if(buf_size < 0)
            return buf_size;

        my_write(1, buf, buf_size);
    }

    return 0;
}

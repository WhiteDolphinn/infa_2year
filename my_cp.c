#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FORCE 1
#define INTERACTIVE 2
#define VERBOSE 4
#define HELP 8
#define PRESERVE 16

#define SIZE 8192

char parser(int argc, char* const argv[]);
static void change_cur_flag_if_short_name(char* cur_flag);
ssize_t my_write(int fd, void* buf, size_t count);
int write_from_fd(int fd_read, int fd_write, size_t count);
static void help();
int copy_file(char * const old_file, char* const new_file, char file);
int count_files(int argc, char* const argv[]);

int main(int argc, char* const argv[])
{
    char flag = parser(argc, argv);
    int num_of_files = count_files(argc, argv);
     printf("flag:\t%x\n", flag);
     printf("num_of_files:\t%d\n", num_of_files);

    if(num_of_files < 2)
    {
        printf("Too few files.\n");
        return 1;
    }

    if(flag & HELP)
        help();

    if(flag & INTERACTIVE)
    flag &= ~FORCE;

    if(num_of_files == 2)
    {
        if(copy_file(argv[argc - 2], argv[argc - 1], flag))
            return 2;
    }
    else
    {
        const char* const dir = argv[argc - 1];

        for(int i = argc - num_of_files; i < argc - 1; i++)
        {
            char new_file_name[SIZE] = {};
            strcpy(new_file_name, dir);
            strcat(new_file_name, "/");
            strcat(new_file_name, argv[i]);
            printf("%s\n", new_file_name);

            if(copy_file(argv[i], new_file_name, flag))
                return 2;
        }
    }

    // printf("flag:\t%x\n", flag);
    return 0;
}


char parser(int argc, char* const argv[])
{
    struct option options[] = {
                                {"force", 0, NULL, FORCE},
                                {"interactive", 0, NULL, INTERACTIVE},
                                {"verbose", 0, NULL, VERBOSE},
                                {"help", 0, NULL, HELP},
                                {"preserve", 0, NULL, PRESERVE},
                                {0, 0, 0, 0}};

    char flag = 0;
    char cur_flag = 0;

    while((cur_flag = getopt_long(argc, argv, "fivhp", options, NULL)) != -1)
    {
        change_cur_flag_if_short_name(&cur_flag);

        flag |= cur_flag;
        cur_flag = 0;
        continue;
    }

     for(int i = 1; i < argc; i++)
     {
         printf("%s\n", argv[i]);
     }

    return flag;
}

int count_files(int argc, char* const argv[])
{
    int i = 1;
    for( ; i < argc; i++)
        if(argv[i][0] != '-')
            break;

    return argc - i;
}

static void change_cur_flag_if_short_name(char* cur_flag)
{
    switch(*cur_flag)
    {
        case 'f':
            *cur_flag = 1;
        break;

        case 'i':
            *cur_flag = 2;
        break;

        case 'v':
            *cur_flag = 4;
        break;

        case 'h':
            *cur_flag = 8;
        break;

        case 'p':
            *cur_flag = 16;
        break;

        default:
        break;
    }
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

int write_from_fd(int fd_read, int fd_write, size_t count)
{
    char buf[SIZE] = {};
    int buf_size = 0;

    while((buf_size = read(fd_read, buf, count)))
    {
        if(buf_size < 0)
            return buf_size;

        my_write(fd_write, buf, buf_size);
    }

    return 0;
}

static void help()
{
    printf("Help:\tВыходит мужик на балкон, а балкона-то и нет:)\n");
}

int copy_file(char * const old_file, char* const new_file, char flag)
{
    if(!strcmp(old_file, new_file))
        return 0;

    int fd_read = open(old_file, O_RDONLY, 00700);
    if(fd_read < 0)
    {
        printf("Cannot find or open file: '%s'\n", old_file);
        return 1;
    }

    int open_flag = O_CREAT | O_RDWR;
    if(flag & FORCE)
        open_flag |= O_TRUNC;

    mode_t open_mode = 0700;
    if(flag & PRESERVE)
    {
        struct stat file_stat = {};
        stat(old_file, &file_stat);
        open_mode |= (file_stat.st_mode & 0700); ////////////////////
    }

    int fd_write = open(new_file, open_flag, open_mode);
    if(fd_write < 0)
    {
        printf("Cannot find or open file: '%s'\n", new_file);
        close(fd_read);
        return 2;
    }

    if(flag & VERBOSE)
        printf("'%s' -> '%s'\n", old_file, new_file);

    if(flag & INTERACTIVE)
    {
        printf("Do you want to rewrite '%s'? Press 'y' if yes.\n", new_file);
        if(getchar() != 'y')
            return 0;
    }

    write_from_fd(fd_read, fd_write, SIZE);
    close(fd_read);
    close(fd_write);

    return 0;
}

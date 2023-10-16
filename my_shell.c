#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

#define MAX_COUNT_PROGS 16
#define MAX_WORD_LENGTH 32
#define MAX_COUNT_WORDS 16
#define SIZE (MAX_WORD_LENGTH * MAX_COUNT_WORDS * MAX_COUNT_PROGS)

int read_command(char* argv);
int execute_command(char argv[MAX_COUNT_PROGS][MAX_COUNT_WORDS][MAX_WORD_LENGTH], int num_of_progs);
ssize_t my_write(int fd, void* buf, size_t count);
int skip_spaces(char* str, int* i);
int write_from_fd(int fd, size_t count);

int main()
{
    const char * const dollar = "$ ";

    while(1)
    {
        write(1, dollar, 3);

        char argv[MAX_COUNT_PROGS][MAX_COUNT_WORDS][MAX_WORD_LENGTH] = {};

        int num_of_progs = read_command((char*)argv);
        if(execute_command(argv, num_of_progs) == 1)
            return 0;

        //printf("%d\n", num_of_progs);
        /*printf("===============\n");
        for(int i = 0; i < MAX_COUNT_PROGS; i++)
        {
            for(int j = 0; j < MAX_COUNT_WORDS; j++)
                printf("\"%s\" ", argv[i][j]);

            printf("\n");
        }
        printf("===============\n");*/

    }

    return 0;
}

int read_command(char* argv)
{
    char str[SIZE] = {};
    read(0, str, SIZE);

    int num_of_progs = 0;

    int last_word_point = 0;
    int num_of_words = 0;

    int i = 0;
    for(; str[i] != '\n' && str[i] != '\0' && i < SIZE; i++)
    {
        if(!skip_spaces(str, &i))
            continue;

        //printf("i = %d\tlast_word_point = %d\t", i, last_word_point);
        strncpy(argv + num_of_progs * MAX_WORD_LENGTH*MAX_COUNT_WORDS + num_of_words*MAX_WORD_LENGTH,
                 str + last_word_point, i - last_word_point - 1);
        num_of_words++;

        if(str[i] == '|')
        {
            num_of_progs++;
            i++;
            skip_spaces(str, &i);
            last_word_point = i;
            num_of_words = 0;
            continue;
        }

        skip_spaces(str, &i);
        last_word_point = i;
       // printf("i = last_word_point = %d\n", i);
    }

    strncpy(argv + num_of_progs * MAX_WORD_LENGTH*MAX_COUNT_WORDS + num_of_words*MAX_WORD_LENGTH,
                 str + last_word_point, i - last_word_point);

    return num_of_progs+1;
}

int execute_command(char argv[MAX_COUNT_PROGS][MAX_COUNT_WORDS][MAX_WORD_LENGTH], int num_of_progs)
{
    //char buf[SIZE] = {};

    /*int fds[2] = {};
    int fds2[2] = {};*/
    int fds[num_of_progs][2];

    for(int i = 0; i < num_of_progs; i++)
    {
        pipe(fds[i]);
        pid_t pid = fork();

        if(pid == 0)
        {
            //my_write(0, buf, SIZE);
            //close(fds[0]);
            /*dup2(fds[0], 0);
            close(fds[0]);*/
            if(i > 0)
                dup2(fds[i-1][0], 0);

            if(i < num_of_progs - 1)
                dup2(fds[i][1], 1);
            //close(fds[1]);

            //my_write(fds2[1], buf, SIZE);

            //execvp(argv[1], argv + 1);
            char** arggv = (char**)calloc(MAX_COUNT_WORDS, sizeof(char*));

            int j = 0;
            for(; j < MAX_COUNT_WORDS; j++)
            {
                    arggv[j] = argv[i][j];

                    if(arggv[j][0] == '\0')
                    {
                        arggv[j] = NULL;
                        break;
                    }
            }

            if(execvp(argv[i][0], arggv) < 0)
            {
                char *errorbuf = strerror(errno);
                perror(errorbuf);
            }

            free(arggv);
            return 1;
        }
        close(fds[i][1]);
    }

    for(int i = 0; i < num_of_progs; i++)
    {
        close(fds[i][0]);
        close(fds[i][1]);
    }

    for(int i = 0; i < num_of_progs; i++)
        wait(NULL);

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

int skip_spaces(char* str, int* i)
{
    int old_i = *i;

    while(str[*i] == ' ' && (*i) < SIZE - 1)
        (*i)++;

    return (*i) - old_i;
}

int write_from_fd(int fd, size_t count)
{
    char buf[SIZE] = {};
    int buf_size = 0;

    while((buf_size = read(fd, buf, count)))
    {
        if(buf_size < 0)
            return buf_size;

        my_write(1, buf, buf_size);
    }

    return 0;
}

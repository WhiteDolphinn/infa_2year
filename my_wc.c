#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <ctype.h>

#define SIZE 131072

int main(int argc, char* const* argv)
{
    int fds[2] = {};
    pipe(fds);

    pid_t pid = fork();

    if(pid == 0)
    {
        dup2(fds[1], 1);
        close(fds[1]);
        execvp(argv[1], argv + 1);
        return 0;
    }
    wait(NULL);
    close(fds[1]);

    int num_of_chars = 0;
    int num_of_lines = 0;
    int num_of_words = 0;
    char buf[SIZE] = {};

    int current_read_count = 0;
    while((current_read_count = read(fds[0], buf + num_of_chars, SIZE)))
    {
        if(current_read_count < 0)
            perror("Error in read.");

        printf("current_read_count = %d\n", current_read_count);
        num_of_chars += current_read_count;
    }

    printf("num_of_chars = %d\n", num_of_chars);

    for(int i = 0; i < num_of_chars; i++)
    {
        if(buf[i] == '\n')
            num_of_lines++;

        if(isspace(buf[i]))
            num_of_words++;
    }




    printf("num_of_lines = %d\n", num_of_lines);
    printf("num_of_words = %d\n", num_of_words);

    return 0;
}

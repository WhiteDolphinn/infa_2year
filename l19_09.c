#include <stdio.h>
#include <unistd.h>

int main()
{
    char* p = NULL;
    printf("hello");
    write(1, "world", 5);
    fflush(stdout);
    *p = 'h';
    printf("good bye");
    write(1, "sea", 3);
    return 0;
}

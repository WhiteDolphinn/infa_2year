#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
//#include <fcntl.h>

#define SIZE 1024

void print_dir(const char* argv_i);

int main(int argc, const char** argv)
{
    for(int i = 1; i < argc; i++)
        print_dir(argv[i]);
    return 0;
}

void print_dir(const char* argv_i)
{
    DIR* dp = opendir(argv_i);

    if(dp == 0)
        return;


    struct dirent* ep = readdir(dp);

    //printf("%s", argv_i);

    while(ep != NULL)
    {
        if((ep->d_name[0] == '.' && ep->d_name[1] == '.' && ep->d_name[2] == '\0')
            || (ep->d_name[0] == '.' && ep->d_name[1] == '\0'))
        {
            ep = readdir(dp);
            continue;
        }

        if(ep->d_type == DT_DIR)
        {
            char buf[SIZE] = {};
            sprintf(buf, "%s/%s", argv_i, ep->d_name);
            print_dir(buf);
        }

        printf("%s/%s\n", argv_i, ep->d_name);

        ep = readdir(dp);
    }

    closedir(dp);
    //printf("\n");
}


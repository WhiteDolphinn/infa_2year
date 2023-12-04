#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#define SIZE 32

void get_user_info(const char* user_name);

int main(int argc, const char** argv)
{
    if(argc == 1)
    {
        uid_t uid = getuid();
        struct passwd* user = getpwuid(uid);
        get_user_info(user->pw_name);
        return 0;
    }

    for(int i = 1; i < argc; i++)
        get_user_info(argv[i]);
}

void get_user_info(const char* user_name)
{
    struct passwd* pass = NULL;
    __gid_t gr[SIZE] = {};
    int count_groups = getgroups(SIZE, gr);
    pass = getpwnam(user_name);

    if(!pass)
    {
        printf("id: '%s': no such user\n", user_name);
        return;
    }

    getgrouplist(pass->pw_name, pass->pw_gid, gr, &count_groups);


    printf("uid=%d(%s) ", (int)pass->pw_uid, pass->pw_name);
    printf("gid=%d(%s) ", (int)pass->pw_gid, pass->pw_name);

    printf("groups=");
    for(int i = 0; i < count_groups; i++)
    {
    printf("%d", gr[i]);
    struct group* group = getgrgid(gr[i]);
    printf("(%s),", group->gr_name);
    }

    printf("\n");
}

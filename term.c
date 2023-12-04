#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

void win_print()
{
    struct winsize win = {};
    ioctl(0, TIOCGWINSZ, &win);
    printf("\033[H\033[J");
    printf("\033[%d;%dH %d rows, %d cols\n", win.ws_row/2, win.ws_col/2, win.ws_row, win.ws_col);
}

void bye_print()
{
    struct winsize win = {};
    ioctl(0, TIOCGWINSZ, &win);
    printf("\033[H\033[J");
    printf("\033[%d;%dH Good bye!\n", win.ws_row/2, win.ws_col/2);
    exit(0);
}

int main()
{

    signal(SIGWINCH, win_print);
    signal(SIGINT, bye_print);

    while(1)
        sleep(3);

    return 0;
}

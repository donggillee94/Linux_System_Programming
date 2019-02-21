#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main()
{
    pid_t pid;

    if((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(1);
    }
    else if(pid == 0)
        execl("/home/leedonggil/test5", "./test","process", "running", (void *)0);
    else
        sleep(3);
    kill(pid, SIGINT);
    exit(0);
}

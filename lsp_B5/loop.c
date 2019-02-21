#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void ssu_handler1(int signo);
void ssu_handler2(int signo);

void ssu_handler1(int signo) {
    if(signo == SIGKILL)
        printf("SIGKILL caught\n");
    else if(signo == SIGTERM)
        printf("SIGTERM caught\n");
    else {
        fprintf(stderr, "unexpected signal\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
int main()
{
    struct sigaction sig_act;

    sig_act.sa_flags = 0;
    sig_act.sa_handler = ssu_handler1;
    sigemptyset(&sig_act.sa_mask);
    sigaction(SIGTERM, &sig_act, NULL);
    sigaction(SIGKILL, &sig_act, NULL);
    while(1) {
         ;
    }
     exit(0);
}

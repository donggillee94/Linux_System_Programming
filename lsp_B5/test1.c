#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo) {

    printf("ssu_signal_handler control\n");
    while(1) {
        sleep(3);
        kill(getpid(), SIGINT);
    }
}
int main()
{
    struct sigaction sig_act;

    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = 0;
    sig_act.sa_handler = ssu_signal_handler;
    sigaddset(&sig_act.sa_mask, SIGINT);
    sigaction(SIGINT, &sig_act, NULL);
    kill(getpid(), SIGINT);
}

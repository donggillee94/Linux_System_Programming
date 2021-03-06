#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal(int signo) {
    printf("SIGUSR1 catched\n");
}

int main()
{
    pid_t pid;
    sigset_t sigset;
    sigset_t pending_sigset;
    
    signal(SIGUSR1, ssu_signal);
    sigfillset(&sigset);
    sigprocmask(SIG_BLOCK, &sigset, 0);
    kill(getpid(), SIGUSR1);

    if((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(0);
    }
    else if(pid == 0) {
        sigpending(&pending_sigset);

        if(sigismember(&pending_sigset, SIGUSR1))
            printf("child : SIGUSR1 pending\n");
    }
    else{
        sigpending(&pending_sigset);
        if(sigismember(&pending_sigset, SIGUSR1))
            printf("parent : SIGUSR1 pending\n");
    }
    exit(0);
}

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int ssu_daemon_init(void);

int main()
{
    if(ssu_daemon_init() < 0) {
        fprintf(stderr, "ssu_daemon_init error\n");
        exit(1);
    }

    openlog("ex8", 0, LOG_LPR);
    syslog(LOG_INFO, "My pid is %d", getpid());
    closelog();

    while(1){
    }

    exit(0);
}

int ssu_daemon_init() {
    pid_t pid;
    int fd, maxfd;

    if((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(1);
    }
    else if(pid != 0)
        exit(0);

    setsid();
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    maxfd = getdtablesize();

    for(fd = 0; fd < maxfd; fd++)
        close(fd);

    umask(0);
    chdir("/");
    fd = open("/dev/null", O_RDWR);
    dup(0);
    dup(0);

    return 0;
}


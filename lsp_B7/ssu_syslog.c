#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>

int ssu_daemon_init(void);

int main()
{
    printf("daemon process intitialization\n");

    if(ssu_daemon_init() < 0) { //디몬 프로세스 생성
        fprintf(stderr, "ssu_daemon_init failed\n");
        exit(1);
    }


    while(1) { //5초마다 syslog에 로그 작성
        openlog("lpd", LOG_PID, LOG_LPR);
        syslog(LOG_ERR, "open failed lpd %m");
        closelog();
        sleep(5);
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

    pid = getpid();
    printf("process %d running as daemon\n", pid);
    setsid();
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    maxfd = getdtablesize();

    for(fd = 0; fd < maxfd; fd++){
        close(fd);
    }
    umask(0);
    chdir("/");
    fd = open("/dev/null", O_RDWR);
    dup(0);
    dup(0);
    return 0;
}

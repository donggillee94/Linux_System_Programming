#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int ssu_daemon_init(void);

int main()
{
	pid_t pid;

	pid = getpid();
	printf("parent process : %d\n", pid);
	printf("daemon process initialization\n");

	if(ssu_daemon_init() < 0) {
		fprintf(stderr, "ssu_daemon_init failed\n");
		exit(1);
	}
	exit(0);
}

int ssu_daemon_init(void) {
	pid_t pid;
	int fd, maxfd;

	if((pid = fork()) < 0){ //fork후 부모 프로세스는 exit
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid != 0)
		exit(0);

	pid = getpid();
	printf("process %d running as daemon\n", pid);
	setsid();//독립된 session 생성
	signal(SIGTTIN, SIG_IGN);//디몬 프로세스가 불필요한 간섭없이 수행되도록 시그널 무시하게 함
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	maxfd = getdtablesize();

	for(fd = 0; fd < maxfd; fd++) //모든 파일 디스크립터 다 닫아줌
		close(fd);

	umask(0); //umask를 0으로 설정
	chdir("/"); //작업디렉토리는 루트로
	fd = open("/dev/null", O_RDWR); //표준 입출력 ,에러를 /dev/null로 재지정
	dup(0);
	dup(0);
	return 0;
}

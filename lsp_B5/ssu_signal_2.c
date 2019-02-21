#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void ssu_signal_handler(int signo);

int main()
{
	if(signal(SIGINT, ssu_signal_handler) == SIG_ERR) {  //SIGINT핸들러 등록
		fprintf(stderr, "cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}

	if(signal(SIGTERM, ssu_signal_handler) == SIG_ERR) { //SIGTERM핸들러 등록
		fprintf(stderr, "cannot handle SIGTERM\n"); 
		exit(EXIT_FAILURE);
	}

	if(signal(SIGPROF, SIG_DFL) == SIG_ERR) { //SIGPROF 디폴트 행동 수행
		fprintf(stderr, "cannot handle SIGPROF\n");
		exit(EXIT_FAILURE);
	}

	if(signal(SIGHUP, SIG_IGN) == SIG_ERR) { //SIGHUP 디폴트 행동 수행
		fprintf(stderr, "cannot handle SIGHUP\n");
		exit(EXIT_FAILURE);
	}

	while(1)
		pause();

	exit(0);
}

static void ssu_signal_handler(int signo) {
	if(signo == SIGINT) //SIGINT 시그널에 대한 처리
		printf("caught SIGINT\n");
	else if(signo == SIGTERM) //SIGTERM 시그널에 대한 처리
		printf("caught SIGTERM\n");
	else { //그 외의 시그널들 처리
		fprintf(stderr, "unexpected signal\n"); 
		exit(EXIT_FAILURE); //비정상적인 종료 
	}
	exit(EXIT_SUCCESS); //성공적으로 종료
}

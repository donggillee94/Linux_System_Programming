#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void ssu_signal_handler1(int signo);
void ssu_signal_handler2(int signo);

int main()
{
	if(signal(SIGINT, ssu_signal_handler1) == SIG_ERR) { //SIGINT에 대해 핸들러 설정
		fprintf(stderr, "cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}

	if(signal(SIGUSR1, ssu_signal_handler2) == SIG_ERR) { //SIGUSR1핸들러 설정
		fprintf(stderr, "cannont handle SIGUSR1\n");
		exit(EXIT_FAILURE);
	}

	raise(SIGINT);//자기 자신에게 SIGINT시그널 보냄
	raise(SIGUSR1); //자기 자신에게 SIGUSR1시그널 보냄
	printf("main return\n");
	exit(0);
}

void ssu_signal_handler1(int signo) {
	printf("SIGINT 시그널 발생\n");
}

void ssu_signal_handler2(int signo) {
	printf("SIGUSR1 시그널 발생\n");
}

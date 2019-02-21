#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo);

void (*ssu_func)(int);

int main()
{
	ssu_func = signal(SIGINT, ssu_signal_handler); //시그널 지정

	while(1) {
		printf("process running...\n");
		sleep(1);
	}

	exit(0);
}

void ssu_signal_handler(int signo) { //SIGINT시그널 발생시 수행
	printf("SIGINT 시그널 발생.\n");
	printf("SIGINT를  SIG_DFL로 재설정 함.\n");
	signal(SIGINT, ssu_func); //재지정
}

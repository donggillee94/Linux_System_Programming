#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo) {
	printf("ssu_signal_handler control\n");
}

int main()
{
	struct sigaction sig_act;
	sigset_t sig_set;

	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sig_act.sa_handler = ssu_signal_handler;
	sigaction(SIGUSR1, &sig_act, NULL); //SIGUSR1 시그널을 마스크해줌
	printf("before first kill()\n");
	kill(getpid(), SIGUSR1); //SIGUSR1 발생
	sigemptyset(&sig_set);//sig_set 초기화
	sigaddset(&sig_set, SIGUSR1); //sig_set에 SIGUSR1 추가
	sigprocmask(SIG_SETMASK, &sig_set, NULL); //시그널집합의 모든 시그널 마스크
	printf("before second kill()\n");
	kill(getpid(), SIGUSR1); //SIGUSR1 발생
	printf("after second kill()\n");
	exit(0);
}

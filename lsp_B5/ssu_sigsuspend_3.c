#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void ssu_signal_handler(int signo);
void ssu_timestamp(char *str);

int main()
{
	struct sigaction sig_act;
	sigset_t blk_set;

	sigfillset(&blk_set); //모든 시그널을 blk_set시그널 집합에 추가
	sigdelset(&blk_set, SIGALRM); //SIGALRM 삭제
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sig_act.sa_handler = ssu_signal_handler; //핸들러 설정
	sigaction(SIGALRM, &sig_act, NULL); //SIGALRM 설정
	ssu_timestamp("before sigsuspend()");
	alarm(5);
	sigsuspend(&blk_set); //SIGALRM이 발생하기전까지 프로세스 중지
	ssu_timestamp("after sigsuspend()");
	exit(0);
}

void ssu_signal_handler(int signo) {
	printf("in ssu_signal handler() function\n");
}

void ssu_timestamp(char *str) {
	time_t time_val;

	time(&time_val);
	printf("%s the time is %s\n", str, ctime(&time_val));
}

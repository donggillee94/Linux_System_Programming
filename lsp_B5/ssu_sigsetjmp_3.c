#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

static void ssu_signal_handler1(int signo);
static void ssu_signal_handler2(int signo);

sigjmp_buf jmp_buf1;
sigjmp_buf jmp_buf2;

int main()
{
	struct sigaction act_sig1;
	struct sigaction act_sig2;
	int i, ret;

	printf("My PID is  %d\n", getpid());
	ret = sigsetjmp(jmp_buf1, 1); //jmp지정 설정 ret에 longjmp로 부터 받은 값 저장

	if(ret == 0) { //ret가 0일 경우
		act_sig1.sa_handler = ssu_signal_handler1;
		sigaction(SIGINT, &act_sig1, NULL);
	}
	else if(ret == 3) //ret가 3일 경우
		printf("----------------\n");

	printf("Starting\n");
	sigsetjmp(jmp_buf2, 2); //ssu_signal_handler2의 jmp지점
	act_sig2.sa_handler = ssu_signal_handler2;
	sigaction(SIGUSR1, &act_sig2, NULL);

	for(i = 0; i < 20; i++) {
		printf("i = %d\n", i);
		sleep(1);
	}

	exit(0);
}

static void ssu_signal_handler1(int signo) {
	fprintf(stderr, "\nInterrupted\n");
	siglongjmp(jmp_buf1, 3);
}

static void ssu_signal_handler2(int signo) {
	fprintf(stderr, "\nSIGUSR1\n");
	siglongjmp(jmp_buf2, 2);
}

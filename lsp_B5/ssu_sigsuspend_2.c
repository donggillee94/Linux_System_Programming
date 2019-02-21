#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

static void ssu_func(int signo);
void ssu_print_mask(const char *str);

int main()
{
	sigset_t new_mask, old_mask, wait_mask;
	
	ssu_print_mask("program start: "); //ssu_print_mask 호출

	if(signal(SIGINT, ssu_func) == SIG_ERR) { //SIGINT시그널 핸들러 설정
		fprintf(stderr, "signal(SIGINT) error\n");
		exit(1);
	}
    signal(SIGUSR2, ssu_func);
	sigemptyset(&wait_mask); //wait_mask 초기화
	sigaddset(&wait_mask, SIGUSR1); //wait_mask에 SIGUSR1 시그널 추가
	sigemptyset(&new_mask); //new_mask 초기화
	sigaddset(&new_mask, SIGINT); //new_mask에 SIGINT 시그널 추가

	if(sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) { //old_mask에 new_mask를 블록시킨 상태로 합함
		fprintf(stderr, "SIG_BLOCK() error\n");
		exit(1);
	}

	ssu_print_mask("in critical region: "); //ssu_print_mask 호출

	if(sigsuspend(&wait_mask) != -1) { //SIGUSR1을 시그널 마스크로 설정 후 종료 시그널 들어올때까지 프로세스 중지
		fprintf(stderr, "sigsuspend() error\n");
		exit(1);
	}

	ssu_print_mask("after return from sigsuspend: "); //ssu_print_mask 호출

	if(sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
		fprintf(stderr, "SIG_SETMASK() error\n");
		exit(1);
	}

	ssu_print_mask("program exit: ");
	exit(0);
}

void ssu_print_mask(const char *str) {
	sigset_t sig_set;
	int err_num;

	err_num = errno;

	if(sigprocmask(0, NULL, &sig_set) < 0) { //현재 시그널 집합 가져옴
		fprintf(stderr, "sigprocmask() error\n");
		exit(1);
	}

	printf("%s", str);

	if(sigismember(&sig_set, SIGINT)) //현재 시그널 목록 찾아서 출력해줌
		printf("SIGINT ");

	if(sigismember(&sig_set, SIGQUIT))
		printf("SIGQUIT ");

	if(sigismember(&sig_set, SIGUSR1))
		printf("SIGUSR1 ");

	if(sigismember(&sig_set, SIGALRM))
		printf("SIGALRM ");

	printf("\n");
	errno = err_num;
}

static void ssu_func(int signo) {
	ssu_print_mask("\nin ssu_func: ");
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main()
{
	sigset_t old_set;
	sigset_t sig_set;

	sigemptyset(&sig_set); //sig_set초기화
	sigaddset(&sig_set, SIGINT); //sig_set에 SIGINT 시그널 추가
    for(int i = 1; i <= 30; i++) {
        if(sigismember(&old_set, i))
            printf("%d is in set\n", i);
    }

	sigprocmask(SIG_BLOCK, &sig_set, &old_set); //SIGINT를 블록시킨 시그널 집합을 old_set에 저장

	sigsuspend(&old_set); //SIGINT가 발생할때까지 프로세스 일시정지
	exit(0);
}

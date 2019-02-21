#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main()
{
	sigset_t pendingset;
	sigset_t sig_set;
	int count = 0;

	sigfillset(&sig_set); //모든 시그널을 sig_set에 포함
	sigprocmask(SIG_SETMASK, &sig_set, NULL); //sig_set에 포함되어있는 모든 시그널을 블럭상태로 해줌

	while(1) {
		printf("count : %d\n", count++);
		sleep(1);

		if(sigpending(&pendingset) == 0) {//현재 유보중이거나 블럭상태인 시그널을 찾아냄
			if(sigismember(&pendingset, SIGINT)) {//SIGINT발생시 해당 시그널을 확인 후 무한루프 종료시켜줌
				printf("SIGINT가 블록되어 대기중. 무한 루프를 종료.\n");
				break;
			}
		}
	}

	exit(0);
}

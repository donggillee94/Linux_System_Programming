#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main()
{
	sigset_t sig_set;
	int count;

	sigemptyset(&sig_set); //시그널집합 초기화
	sigaddset(&sig_set,SIGINT); //시그널집합에 SIGINT 추가해줌
	sigprocmask(SIG_BLOCK, &sig_set, NULL); //보유중인 시그널들 블록시켜줌

	for(count = 3; 0 < count; count--) {
		printf("count %d\n", count);
		sleep(1);
	}

	printf("Ctrl-C에 대한 블록을 해제\n");
	sigprocmask(SIG_UNBLOCK, &sig_set, NULL);//SIGINT 블록해제 해줌
	printf("count중 Ctrl-C입력하면 이 문장은 출력 되지 않음.\n");

	while(1);
	
	exit(0);
}

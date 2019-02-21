#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_alarm(int signo);

int main()
{
	printf("Alarm Setting\n");
	signal(SIGALRM, ssu_alarm);
	alarm(2);

	while(1) {
		printf("done\n");
		pause();//SIGALRM을 받을떄까지 프로세스의 수행 중지
		alarm(2); //2초후 SIGALRM 발생
	}

	exit(0);
}

void ssu_alarm(int signo) { //SIGALRM시 수행하는 함수
	printf("alarm..!!!\n");
}

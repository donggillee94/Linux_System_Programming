#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo);

int count = 0;

int main()
{
	signal(SIGALRM, ssu_signal_handler);
	alarm(1);//SIGALRM 1초후 발생

	while(1);//반복문 수행
	
	exit(0);
}

void ssu_signal_handler(int signo) {
	printf("alarm %d\n", count++);
	alarm(1);//1초후 SIGALRM 발생
}

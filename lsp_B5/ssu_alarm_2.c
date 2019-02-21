#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define LINE_MAX 2048

static void ssu_alarm(int signo);

int main()
{
	char buf[LINE_MAX];
	int n;

	if(signal(SIGALRM, ssu_alarm) == SIG_ERR) { //SIGALRM핸들러 설정
		fprintf(stderr, "SIGALRM error\n");
		exit(1);
	}

	alarm(10); //10초후 SIGALRM발생

	if((n = read(STDIN_FILENO, buf, LINE_MAX)) < 0) {//표준입력으로부터 문자열 입력받음
		fprintf(stderr, "read() error\n");
		exit(1);
	}

	alarm(0); //0초후 SIGALRM보냄
	write(STDOUT_FILENO, buf, n) //입력받은 문자열 터미널에 출력
	exit(0);
}

static void ssu_alarm(int signo) {
	printf("ssu_alarm() called\n");
}

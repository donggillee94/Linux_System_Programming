#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

void ssu_signal_handler(int signo);

jmp_buf jump_buffer;

int main()
{
	signal(SIGINT, ssu_signal_handler); //SIGINT에 대한 핸들러 설정

	while(1) {
		if(setjmp(jump_buffer) == 0) {  //jump지점 설정
			printf("Hit Ctrl-c at anytime...\n");
			pause(); //시그널 발생할때까지 중지
		}
	}
	exit(0);
}

void ssu_signal_handler(int signo) {
	char character;

	signal(signo, SIG_IGN); //SIGINT발생하였을시 시그널 무시
	printf("Did tou hit Ctrl-c?\n" "Do you really want to quit? [y/n] ");
	character = getchar();

	if(character == 'y' || character == 'Y') //y, Y입력시 종료
		exit(0);
	else {
		signal(SIGINT, ssu_signal_handler); //그 외에 SIGINT 발생시 시그널 핸들러 설정
		longjmp(jump_buffer, 1); //jump지점으로 jump
	}
}

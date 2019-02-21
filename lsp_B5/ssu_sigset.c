#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
	sigset_t set;
	sigemptyset(&set); //시그널집합 초기화
	sigaddset(&set, SIGINT); //해당 시그널집합에 SIGINT 추가

	switch(sigismember(&set, SIGINT)) //SIGINT시그널이 있는지 확인
	{
		case 1:
			printf("SIGINT is included. \n");
			break;
		case 0:
			printf("SIGINT is not included. \n");
			break;
		default:
			printf("failed to call sigismember() \n");
	}

	switch(sigismember(&set, SIGSYS)) //SIGSYS시그널이 있는지 확인
	{
		case 1:
			printf("SIGSYS is included. \n");
			break;
		case 0:
			printf("SIGSYS is not included. \n");
			break;
		default:
			printf("failed to call sigismember() \n");
	}

	exit(0);
}

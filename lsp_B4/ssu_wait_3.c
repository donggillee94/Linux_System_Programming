#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct timeval bgn, end;

int main()
{
	gettimeofday(&bgn, NULL);
	if(fork() == 0) //자식 프로세스 생성
		execl("/bin/echo", "echo", "this is", "message one", (char *)0); //생성된 자식프로세스 바로 exec
	if(fork() == 0) //자식 프로세스 생성
		execl("/bin/echo", "echo", "this is", "message two", (char *)0); //생성된 자식프로세스 바로 exec

	printf("parent : waiting for children\n");

	while(wait((int *)0) != -1); //모든 자식 프로세스 종료될때까지 부모프로세스 대기

	printf("parent : all children terminated\n");
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", (end.tv_sec - bgn.tv_sec)*1000000+(end.tv_usec - bgn.tv_usec));
	exit(0);
}

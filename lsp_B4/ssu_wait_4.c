#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

struct timeval bgn, end;

int main()
{
	gettimeofday(&bgn, NULL);
	pid_t child1, child2;
	int pid, status;

	if((child1 = fork()) == 0) //첫번째 자식프로세스 생성 후 바로 exec해줌
		execlp("date", "date", (char *)0);

	if((child2 = fork()) == 0) //두번째 자식프로세스 생성 후 바로 exec해줌
		execlp("who", "who", (char *)0);

	printf("parent : waiting for children\n");

	while((pid = wait(&status)) != -1) {//자식 프로세스 모두 종료될때까지
		if(child1 == pid) //첫번째 자식 프로세스의 종료상태 출력
			printf("parent : first child : %d\n", (status >> 8));
		if(child2 == pid) //두번째 자식 프로세스의 종료상태 출력
			printf("parent : second child : %d\n", (status >> 8));
	}

	printf("parent : all childre  terminated\n");
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", (end.tv_sec - bgn.tv_sec)*1000000+(end.tv_usec - bgn.tv_usec));
	exit(0);
}

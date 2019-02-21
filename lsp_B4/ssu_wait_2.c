#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define EXIT_CODE 1

struct timeval bgn, end;

int main()
{
	gettimeofday(&bgn, NULL);
	pid_t pid;
	int ret_val, status;

	if((pid = fork()) == 0) {
		printf("child : pid = %d ppid = %d exit_code = %d\n", getpid(), getppid(), EXIT_CODE); //자식 프로세스id, 부모 프로세스의 id, 종료 상태
		exit(EXIT_CODE);
	}

	printf("parent : waiting for child = %d\n", pid);
	ret_val = wait(&status);//자식 프로세스 종료시까지 부모프로세스 대기, 종료상태 status에 넣어줌
	printf("parent : return value = %d, ", ret_val);
	printf(" child's status = %x", status);
	printf(" and shifted = %x\n", (status >> 8));
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", (end.tv_sec - bgn.tv_sec)*1000000+(end.tv_usec - bgn.tv_usec));
	exit(0);
}

#include <sys/wait.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void ssu_echo_exit(int status);
struct timeval bgn, end;

int main()
{
	gettimeofday(&bgn, NULL);
	pid_t pid;
	int status;

	if((pid = fork()) < 0) { //자식 프로세스 생성
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid == 0) //셍성된 자식프로세스 종료
		exit(7);

	if(wait(&status) != pid) { //해당 자식프로세스 종료시까지 부모프로세스 대기 
		fprintf(stderr, "wait eror\n");
		exit(1);
	}
	ssu_echo_exit(status); //자식 프로세스의 종료상태를 가지고 종료상태 분석 함수 출력

	if((pid = fork()) < 0) { //자식 프로세스 생성
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid == 0) //자식 프로세스 abort로 비정상적 종료 시켜줌
		abort();

	if(wait(&status) != pid) { //자식 프로세스 종료시까지 부모프로세스 대기
		fprintf(stderr, "wait error\n");
		exit(1);
	}

	ssu_echo_exit(status); //자식 프로세스의 종료상태를 가지고 종료상태 분석 함수 출력

	if((pid = fork()) < 0) { //자식 프로세스 생성
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid == 0) //status를 0으로 나누면 비정상적인 종료를 하게 된다
		status /= 0;

	if(wait(&status) != pid) { //해당 비정상적 종료 시그널을 status에 넣어줌
		fprintf(stderr, "wait error\n");
		exit(1);
	}

	ssu_echo_exit(status); //종료 시그널 분석
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", (end.tv_sec - bgn.tv_sec)*1000000+(end.tv_usec - bgn.tv_usec));
	exit(0);
}

void ssu_echo_exit(int status) {
	if(WIFEXITED(status)) //정상적으로 종료시에
		printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
	else if(WIFSIGNALED(status)) //비정상적 종료시에
		printf("abnormal termination, signal number = %d%s\n", WTERMSIG(status),
#ifdef WCOREDUMP
				WCOREDUMP(status) ? " (core file generated)" : "");
#else
				"");
#endif
	else if (WIFSTOPPED(status)) //자식 프로세스가 중지 상태인 경우에
		printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}

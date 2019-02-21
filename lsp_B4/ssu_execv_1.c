#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

double ssu_maketime(struct timeval *time);
void term_stat(int stat);
void ssu_print_child_info(int stat, struct rusage *rusage);
struct timeval bgn, end;

int main()
{
	gettimeofday(&bgn, NULL);
	struct rusage rusage;
	pid_t pid;
	int status;

	if((pid = fork()) == 0) { //자식 프로세스 생성
		char *args[] = {"find", "/", "-maxdepth", "4", "-name", "stdio.h", NULL}; //exec시 사용될 인자들

		if(execv("/usr/bin/find", args) < 0) { //위에서 지정해준 인자들 이용해서 exec해줌
			fprintf(stderr, "exec error\n");
			exit(1);
		}
	}

	if(wait3(&status, 0, &rusage) == pid) //자식 프로세스의 자원 사용량 측정 및 부모 프로세스 대기
		ssu_print_child_info(status, &rusage); //자식 프로세스의 자원 사용량 출력
	else {
		fprintf(stderr, "wait3 error\n");
		exit(1);
	}
	gettimeofday(&end,NULL);
	printf("elapsed time : %ld us\n", (end.tv_sec - bgn.tv_sec)*1000000+(end.tv_usec - bgn.tv_usec));
	exit(0);
}

double ssu_maketime(struct timeval *time) {
	return ((double)time -> tv_sec + (double) time -> tv_usec/1000000.0);
}

void term_stat(int stat) {
	if(WIFEXITED(stat)) //정상적 종료시
		printf("normally terminated. exit status = %d\n", WEXITSTATUS(stat));
	else if(WIFSIGNALED(stat)) //비정상적 종료시
		printf("abnormal termination by signal %d. %s\n", WTERMSIG(stat),
#ifdef WCOREDUMP
				WCOREDUMP(stat)?"core dumped":"no core"
#else
				NULL
#endif
			  );
	else if(WIFSTOPPED(stat))
		printf("stopped by signal %d\n", WSTOPSIG(stat));
}

void ssu_print_child_info(int stat, struct rusage *rusage) {
	printf("Termination info follows\n");
	term_stat(stat); //종료상태 출력 함수
	printf("user CPU time : %.2f(sec)\n", ssu_maketime(&rusage->ru_utime)); //사용자 모드로 CPU사용한 시간 출력
	printf("system CPU time : %.2f(sec)\n", ssu_maketime(&rusage->ru_stime)); //시스템 모드에서 CPU사용한 시간 출력
}

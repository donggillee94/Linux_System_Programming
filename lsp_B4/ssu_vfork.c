#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void ssu_local_func(void);
struct timeval bgn, end;

int main()
{
	printf("Before vfork\n"); //vfork하기 전 print문
	ssu_local_func(); //vfork시행
	printf("After ssu_local_func, my PID is %d\n", getpid());
	_exit(0);
}

void ssu_local_func(void) {
	pid_t pid;

	if((pid = vfork()) == 0) //vfork시행 후 pid가 0 즉 자식 프로세스인 경우
		printf("I'm child. My PID is %d\n", getpid());
	else if(pid > 0) { //부모 프로세스의 경우
        printf("I'm father, My PID is %d\n", getpid());
        sleep(2);
    }
	else
		fprintf(stderr, "vfork error\n");
}

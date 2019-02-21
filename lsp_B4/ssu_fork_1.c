#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char glob_str[] = "write to statndard output\n";
int glob_val = 10;
struct timeval bgn, end;

int main()
{
	gettimeofday(&bgn, NULL);
	pid_t pid;
	int loc_val;

	loc_val = 100;

	if(write(STDOUT_FILENO, glob_str, sizeof(glob_str)-1) != sizeof(glob_str) -1) {//이 부분은 버퍼링 되지 않고 한번 출력됨
		fprintf(stderr, "write error\n");
		exit(1);
	}
	
	printf("before fork\n");//문자열 출력 후 버퍼에 남아있는 상태로 부모, 자식 둘 다 출력됨

	if((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid == 0) {
		glob_val++;
		loc_val++;
	}
	else
		sleep(3);

	printf("pid = %d, glob_val = %d, loc_val = %d\n", getpid(), glob_val, loc_val);//마찬가지로 변수에 대한 정보 출력도 두번일어나지만 부모 프로세스가 sleep하는 사이에 자식 프로세스가 값을 바꿔놓음
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", ((end.tv_sec - bgn.tv_sec)*1000000 + end.tv_usec - bgn.tv_usec));
	exit(0);
}

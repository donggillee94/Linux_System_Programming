#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct timeval bgn, end;

int main()
{
	gettimeofday(&bgn, NULL);
	if(fork() == 0) { //자식 프로세스 생성 후 exec
		execl("/bin/echo", "echo", "this is", "message one", (char *)0); //exec제대로 시행되면 이 밑의 fprintf문구 및 exit은 프로세스가 덮어지는 현상때문에 시행되지 않는다.
		fprintf(stderr, "exec error\n");
		exit(1);
	}

	if(fork() == 0) { //자식 프로세스 생성 후 exec
		execl("/bin/echo", "echo", "this is ", "message two", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}
	
	if(fork() == 0) { //자식 프로세스 생성 후 exec
		execl("/bin/echo", "echo", "this is ", "message three", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}

	printf("Parent program ending\n");
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", (end.tv_sec - bgn.tv_sec)*100000+(end.tv_usec - bgn.tv_usec));
	exit(0);
}

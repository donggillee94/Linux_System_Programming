#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct timeval bgn, end;

int main(int argc, char *argv[])
{
	gettimeofday(&bgn, NULL);
	pid_t pid;
	char character, first, last;
	long i;

	if((pid = fork()) > 0) { //자식 프로세스 생성 후 부모 프로세스의 경우
		first = 'A';
		last = 'Z';
	}
	else if(pid == 0) {
		first = 'a';
		last = 'z';
	} //자식 프로세스의 경우
	else {
		fprintf(stderr, "%s\n", argv[0]);
		exit(1);
	}

	for(character = first; character <= last; character++) { //각각의 경우를 출력해준다 하지만 write를 사용하기에 버퍼링은 없다
		for(i = 0; i <= 100000; i++)
			;
		write(1, &character, 1);
	}

	printf("\n");
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", (end.tv_sec - bgn.tv_sec) * 1000000 + end.tv_usec - bgn.tv_usec);
	exit(0);
}

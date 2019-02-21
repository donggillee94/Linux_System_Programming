#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	pid_t pid;

	if((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid != 0)
		exit(1);

	printf("before pid = %d, sid = %d\n", getpid(), getsid(getpid()));
	setsid(); //독립된 하나의 세션을 생성해줌
	printf("after pid = %d, sid = %d\n", getpid(), getsid(getpid()));
	exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	if(argc != 2) {
		fprintf(stderr, "usage : %s [Process ID]\n", argv[0]);
		exit(1);
	}
	else
		kill(atoi(argv[1]), SIGKILL); //인자가 제대로 들어온 경우에는 해당 프로세스에 SIGKILL시그널 전달

	exit(0);
}

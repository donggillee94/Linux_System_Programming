#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	pid_t pgid;
	pid_t pid;

	if(argc < 2) {
		fprintf(stderr, "usage : %s <file>\n", argv[0]);
		exit(1);
	}

	pid = getpid();
	pgid = getpgid(atoi(argv[1])); //group의 id를 가져옴
	printf("pid : %d, pgid : %d\n", pid, pgid);
	exit(0);
}

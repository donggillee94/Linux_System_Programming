#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	pid_t pgid;
	pid_t pid;

	pid = getpid();
	pgid = getpgrp();
	printf("pid : %d, pgid : %d\n", pid, pgid);
	exit(0);
}

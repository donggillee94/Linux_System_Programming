#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>

#define STDIN_NUMBER 0

static void ssu_sig_hup(int signum);
static void ssu_print_ids(char *name);

int main()
{
	pid_t pid;
	char character;

	ssu_print_ids("parent");
	if((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid > 0) {
		sleep(3);
		exit(0);
	}
	else {
		ssu_print_ids("child"); //자식 프로세스가 일시중지시 부모 프로세스 종료 
		signal(SIGHUP, ssu_sig_hup); //자식 프로세스는 SIGHUP, SIGCONT를 받는다
		kill(getpid(), SIGTSTP);
		ssu_print_ids("child");

		if(read(STDIN_FILENO, &character, 1) != 1) //배경 프로세스 그룹에 놓이기에 자식프로세스는 read불가
			fprintf(stderr, "read error\n");

		exit(0);
	}
}

static void ssu_sig_hup(int signum) {
	printf("ssu_SIG_HUP received, pid = %d\n", getpid());
}

static void ssu_print_ids(char *name) {
	printf("[%s] : pid = %d, ppid = %d, pgrp = %d, tpgrp = %d\n", name, getpid(), getppid(), getpgrp(), tcgetpgrp(STDIN_NUMBER));
	fflush(stdout);
}

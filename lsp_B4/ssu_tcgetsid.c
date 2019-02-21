#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

int main()
{
	pid_t sid_stderr;
	pid_t sid_stdin;
	pid_t sid_stdout;

	sid_stdin = tcgetsid(STDIN_FILENO);

	if(sid_stdin == -1) { //표준입력에 대한 제어 터미널의 세션 ID출력
		fprintf(stderr, "tcgetsid error\n");
		exit(1);
	}
	else
		printf("Session Leader for stdin : %d\n", sid_stdin);

	sid_stdout = tcgetsid(STDOUT_FILENO);

	if(sid_stdout == -1) { //표준출력에 대한 제어 터미널의 세션 ID출력
		fprintf(stderr, "tcgetsid error\n");
		exit(1);
	}
	else
		printf("Session Leader for stdout : %d\n", sid_stdout);

	sid_stderr = tcgetsid(STDERR_FILENO);

	if(sid_stderr == -1) { //표준에러에 대한 제어 터미널의 세션 ID출력
		fprintf(stderr, "tcgetsid error\n");
		exit(1);
	}
	else
		printf("Session Leader for stderr : %d\n", sid_stderr);

	exit(0);
}

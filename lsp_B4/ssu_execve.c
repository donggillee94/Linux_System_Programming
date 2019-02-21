#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	char *argv[] = {"ssu_execl_test_1", "param1", "param2", (char *)0}; //execve에 들어갈 변수들
	char *env[] = { //execve하면서 지정해줄 환경변수들
		"Name = value",
		"nextname = nextvalue",
		"HOME = /home/oslab",
		(char *)0
	};

	printf("this is the original program\n");
	execve("./ssu_execl_test_1", argv, env); //execve를 이용해서 프로그램 환경변수 지정 및 시행
	printf("%s\n", "This line should never get printed\n");
	exit(0);
}

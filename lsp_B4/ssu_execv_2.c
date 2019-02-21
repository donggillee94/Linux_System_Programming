#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct timeval bgn, end;

int main()
{
	char *argv[] = {"ssu_execl_test_1", "param1", "param2", (char *)0}; //ssu_execl_test_1실행 시에 넣어줄 변수들

	printf("this is the original program\n");
	execv("./ssu_execl_test_1", argv); //위에 설정된 값들을 통헤 exec해줌
	printf("%s\n", "This line should never get printed\n"); //exec이 제대로 시행되었을시 해당 프로세스가 원래 프로세스 덮기에 해당 문구 출력안됨
		exit(0);
}

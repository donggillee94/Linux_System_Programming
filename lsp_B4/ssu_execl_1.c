#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	printf("this is the original program\n");
	execl("./ssu_execl_test_1", "ssu_execl_test_1", "param1", "param2", "param3", (char *)0); //exec통해 ssu_execl_test_1 실행
	printf("%s\n", "this line should never get printed\n"); //제대로 시행되면 프로세스가 덮어지기에 출력 안됨
	exit(0);
}

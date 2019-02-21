#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	printf("before system()\n");
	system("pwd"); //system 함수를 이용해 현재 경로를 출력
	printf("after system()\n");
	exit(0);
}

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int fd, state;

	state = seteuid(1000); //euid를 1000으로 바꿔줌

	if(state < 0) {//seteuid 오류시
		fprintf(stderr, "seteuid error\n");
		exit(1);
	}

	if((fd = open("ssu_test.txt", O_CREAT | O_RDWR, S_IRWXU)) < 0) { //ssu_test.txt를 생성, 루트권한으로 생성시 uid가 user1이기에 owner는 user1로 설정되고 group은 root로 생성된다
		fprintf(stderr, "open error\n");
		exit(1);
	}

	close(fd);
	exit(0);
}

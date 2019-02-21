#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	char line[256];
	FILE *fp;
	int uid;

	if(argc < 2) {
		fprintf(stderr, "usage : %s file_name\n", argv[0]);
		exit(1);
	}

	printf("initially uid = %d and euid = %d\n", getuid(), geteuid()); //현재 사용자의 id, euid를 출력해줌
	fp = fopen(argv[1], "r");

	if(fp == NULL) {//첫번째 open
		fprintf(stderr, "first open error for %s\n", argv[1]);
		exit(1);
	}
	else { //open 성공시 파일 내용 출력
		printf("first open successful:\n");

		while(fgets(line, 255, fp) != NULL)
			fputs(line, stdout);

		fclose(fp);
	}

	setuid(uid = getuid());//uid를 실제 사용자 id로 바꿔줌
	printf("after setuid(%d):\n uid = %d and euid = %d\n", uid, getuid(), geteuid());//setuid후의 uid, euid출력
	fp = fopen(argv[1], "r");

	if(fp == NULL) {//두번째 open
		fprintf(stderr, "second open error for %s\n", argv[1]);
		exit(1);
	}
	else {//성공시 파일 내용 출력
		printf("second open successful:\n");
		while(fgets(line, 255, fp) != NULL)
			fputs(line, stdout);

		fclose(fp);
	}
	exit(0);
}

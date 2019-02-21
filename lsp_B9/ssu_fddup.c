#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
    int testfd;
    int fd;

    fd = open("test.txt", O_CREAT);

    testfd = fcntl(fd, F_DUPFD, 10); //open된 test.txt의 파일디스크립터를 10으로 dup해줌
    printf("testfd : %d\n", testfd); //dup된 파일디스크립터 number출력
    testfd = fcntl(fd, F_DUPFD, 10); //open된 test.txt의 파일디스크립터를 다시10으로 dup해줌
    printf("testfd : %d\n", testfd); //dup된 파일디스크립터 number출력

    getchar();
}

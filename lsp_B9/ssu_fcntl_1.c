#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main()
{
    char *filename = "ssu_test.txt";
    int fd1, fd2;
    int flag;
    
    if((fd1 = open(filename, O_RDWR | O_APPEND, 0644)) < 0) { //ssu_test.txt파일을 O_APPEND플래그 포함시켜 open해줌
        fprintf(stderr, "open error for %s\n", filename);
        exit(1);
    }

    if(fcntl(fd1, F_SETFD, FD_CLOEXEC) == -1) { //해당 파일디스크립터 속성에 FD_CLOEXEC 플래그 포함시켜줌
        fprintf(stderr, "fcntl F_SETFD error\n");
        exit(1);
    }

    if((flag = fcntl(fd1, F_GETFL, 0)) == -1) { //해당 파일디스크립터의 속성 플래그들 가져옴
        fprintf(stderr, "fcntl F_GETFL error\n");
        exit(1);
    }
    
    if(flag & O_APPEND) //속성에 O_APPEND 속성이 있는지 확인
        printf("fd1 : O_APPEND is set\n");
    else
        printf("fd1 : O_APPEND is not set\n");

    if((flag = fcntl(fd1, F_GETFD, 0)) == -1) { //해당 파일 디스크립터로부터 FD_CLOEXEC를 리턴받음
        fprintf(stderr, "fcntl F_GETFD error\n");
        exit(1);
    }

    if(flag & FD_CLOEXEC) //해당 플래그 속성에 FD_CLOEXEC이 있는지 확인
        printf("fd1 : FD_CLOEXEC is set\n");
    else
        printf("fd1 : FD_CLOEXEC is not set\n");

    if((fd2 = fcntl(fd1, F_DUPFD, 0)) == -1) { //fd1과 fd2를 DUP시켜줌
        fprintf(stderr, "fcntl F_DUPFD error\n");
        exit(1);
    }

    if((flag = fcntl(fd2, F_GETFL, 0)) == -1) { //fd1의 속성 플래그들을 가져옴
        fprintf(stderr, "fcntl F_GETFL error\n");
        exit(1);
    }

    if(flag & O_APPEND) //O_APPEND 플래그가 있는지 확인
        printf("fd2 : O_APPEND is set\n");
    else
        printf("fd2 : O_APPEND is not set\n");

    if((flag = fcntl(fd2, F_GETFD, 0)) == -1) { //F_GETFD를 통해서 해당 파일 디스크립터로부터 FD_CLOEXEC리턴받음
        fprintf(stderr, "fcntl F_GETFD error\n");
        exit(1);
    }

    if(flag & FD_CLOEXEC) //해당 파일 디스크립터에 FD_CLOEXEC 플래그 포함여부 확인
        printf("fd2 : FD_CLOEXEC is set\n");
    else
        printf("fd2 : FD_CLOEXEC is not set\n"); //F_DUPFD를 해도 FD_CLOEXEC 플래그는 이전되지 않음을 알 수 있다

    exit(0);
}

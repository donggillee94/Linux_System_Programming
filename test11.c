#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    char *filename = "ssu_test.txt";
    int fd1, fd2;
    int flag;

    if((fd1 = open(filename, O_RDWR | O_APPEND, 0644)) < 0) {
        fprintf(stderr, "open error for %s\n", filename);
        exit(1);
    }

    if((flag = fcntl(fd1, F_SETFD, 1)) < 0) {
        fprintf(stderr, "fcntl F_SETFD error\n");
        exit(1);
    }
    
    if((flag = fcntl(fd1, F_GETFL, 0)) < 0) {
        fprintf(stderr, "fcntl F_GETFL error\n");
        exit(1);
    }

    if(flag & O_APPEND)
        printf("fd1 : O_APPEND is set\n");
    else
        printf("fd1 : O_APPEND is NOT set\n");

    if((flag = fcntl(fd1, F_GETFD, 0)) < 0) {
        fprintf(stderr, "fcntl F_GETFD error\n");
        exit(1);
    }
    
    if(flag & FD_CLOEXEC)
        printf("fd1 : FD_CLOEXEC is set\n");
    else
        printf("fd1 : FD_CLOEXEC is NOT set\n");


    if((fd2 = fcntl(fd1, F_DUPFD, 0)) < 0) {
        fprintf(stderr, "fcntl F_DUPFD error\n");
        exit(1);
    }
    
    if((flag = fcntl(fd2, F_GETFL, 0)) < 0) {
        fprintf(stderr, "fcntl F_GETFL error\n");
        exit(1);
    }
    if(flag & O_APPEND)
        printf("fd2 : fcntl O_APPEND is set\n");
    else
        printf("fd2 : fcntl O_APPEND is NOT set\n");

    if((flag = fcntl(fd2, F_GETFD, 0)) < 0) {
        fprintf(stderr, "fcntl F_GETFD error\n");
        exit(1);
    }

    if(flag & FD_CLOEXEC)
        printf("fd2 : fcntl FD_CLOEXEC is set\n");
    else
        printf("fd2 : fcntl FD_CLOEXEC is NOT set\n");

    exit(0);
}

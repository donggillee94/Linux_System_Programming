#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int fd;
    int val;

    if((fd = open("exec_copy.txt", O_CREAT)) < 0) { //exec_copy.txt파일 열어줌(없을시 생성)
        fprintf(stderr, "open error for %s\n", "exec_copy.txt");
        exit(1);
    }

    val = fcntl(fd, F_GETFD, 0); //해당 파일의 디스크립터를 이용해서 F_GETFD

    if(val & FD_CLOEXEC) //해당 파일의 속성에 FD_CLOEXEC이 있는지 확인
        printf("close-on-exec bit on\n");
    else
        printf("close-on-exec bit off\n");

    val |= FD_CLOEXEC; //해당 파일의 속성에 FD_CLOEXEC 추가해줌

    if(val & FD_CLOEXEC)
        printf("close-on-exec bit on\n");
    else
        printf("close-on-exec bit off\n");

    fcntl(fd, F_SETFD, val); //해당 파일의 속성을 셋팅해줌
    execl("/home/leedonggil/lsp_B9/ssu_loop", "./ssu_loop", NULL); //loop 프로그램 실행시켜줌
    exit(0);
}

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd;
    int val;
    fd = open("ssu_test.txt", O_CREAT); //ssu_test.txt open해줌

    val = fcntl(fd, F_GETFD, 0); //해당 파일디스크립터의 플래그 가져옴
    if(val & FD_CLOEXEC) //CLOSE_ON_EXEC 플래그 포함여부 확인
        printf("close-on-exec bit on\n");
    else
        printf("close-on-exec bit off\n");


    val |= FD_CLOEXEC; //CLOSE_ON_EXEC 플래그 포함시켜줌
    if(val & FD_CLOEXEC)
        printf("close-on-exec bit on\n");
    else
        printf("close-on-exec bit off\n");
    fcntl(fd, F_SETFD, val);//CLOSE_ON_EXEC가 포함된 플래그로 셋팅해줌

    execl("/home/leedonggil/lsp_B9/ssu_loop", "./ssu_loop", NULL);
    exit(0);
}

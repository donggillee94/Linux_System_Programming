#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int fd;
    int val;
    
    fd = open("exec_copy.txt", O_CREAT); //exec_copy.txt파일 열어줌(없을시 생성)
    execl("/home/leedonggil/lsp_B9/ssu_loop", "./ssu_loop", NULL);//ssu_loop 프로그램 실행
    exit(0);
}

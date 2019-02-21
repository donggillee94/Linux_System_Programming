#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_CALL 100
#define MAX_BUF 20000

void serror(char *str)
{
    fprintf(stderr, "%s\n", str);
    exit(1);
}

int main()
{
    int nread, nwrite, val, i = 0;
    char *ptr;
    char buf[MAX_BUF];

    int call[MAX_CALL];

    nread = read(STDIN_FILENO, buf, sizeof(buf)); //표준입력으로부터 read
    fprintf(stderr, "read %d bytes\n", nread);

    if((val = fcntl(STDOUT_FILENO, F_GETFL, 0)) < 0) //표준출력 파일디스크립터의 플래그 가져옴
        serror("fcntl F_GETFL error");
    val |= O_NONBLOCK; //표준출력 파일디스크립터의 속성에 NONBLOCK 추가
    if(fcntl(STDOUT_FILENO, F_SETFL, val) < 0)
        serror("fcntl F_SETFL error");

    for(ptr = buf; nread > 0; i++) {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, nread); //표준출력을 통해 읽어들인 내용 출력

        if(i < MAX_CALL)
            call[i] = nwrite;
        fprintf(stderr,"nwrite = %d, errno = %d\n", nwrite, errno);
        if(nwrite > 0) {
            ptr += nwrite;
            nread -= nwrite;
        }
    }
    
    if((val = fcntl(STDOUT_FILENO, F_GETFL, 0)) < 0)
        serror("fcntl F_GETFL error");
    val &= ~O_NONBLOCK; //표준출력 파일디스크립터의 NONBLOCK 해제
    if(fcntl(STDOUT_FILENO, F_SETFL, val) < 0)
        serror("fcntl F_SETFL error");

    for(i = 0; i < MAX_CALL; i++)
        fprintf(stderr,"call[%d] = %d\n", i, call[i]);

    exit(0);
}

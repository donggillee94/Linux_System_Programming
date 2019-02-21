#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

void set_flags(int fd, int flags);
void clr_flags(int fd, int flags);

char buf[500000];

int main()
{
    int ntowrite, nwrite;
    char *ptr;

    ntowrite = read(STDIN_FILENO, buf, sizeof(buf)); //표준입력으로부터 읽어들임
    fprintf(stderr, "reading %d bytes\n", ntowrite); //몇바이트 읽었는지 표준에러로 출력

    set_flags(STDOUT_FILENO, O_NONBLOCK); //nonblocking 설정

    ptr = buf;

    while(ntowrite > 0) {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, ntowrite); //표준출력으로 buf에 있는 내용 출력
        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

        if(nwrite > 0) {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }
    clr_flags(STDOUT_FILENO, O_NONBLOCK); //nonblocking 해제
    exit(0);
}

void set_flags(int fd, int flags)
{
    int val;

    if((val = fcntl(fd, F_GETFL, 0)) < 0) { //해당 파일디스크립터의 플래그를 가져온다
        fprintf(stderr, "fcntl F_GETFL failed");
        exit(1);
    }

    val |= flags;

    if(fcntl(fd, F_SETFL, val) < 0) { //flag를 포함시켜줌
        fprintf(stderr, "fcntl F_SETFL failed");
        exit(1);
    }

}

void clr_flags(int fd, int flags) {
    int val;

    if((val = fcntl(fd, F_GETFL, 0)) < 0) { 
        fprintf(stderr, "fcntl F_GETFL failed");
        exit(1);
    }

    val &= ~flags;

    if(fcntl(fd, F_SETFL, val) < 0) { //해당 파일디스크립터에서 flag를 빼줌
        fprintf(stderr, "fcntl F_SETFL failed");
        exit(1);
    }
}

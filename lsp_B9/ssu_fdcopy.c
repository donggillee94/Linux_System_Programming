#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MSG "message will be written to Terminal\n"

int main()
{
    int new_fd;

    if((new_fd = fcntl(STDOUT_FILENO, F_DUPFD, 3)) == -1) { //표준출력 파일디스크립터를 3으로 복사
        fprintf(stderr, "Error : Copying File Descriptor\n");
        exit(1);
    }

    close(STDOUT_FILENO); //표준출력 파일디스크립터 close해줌
    write(3, MSG, strlen(MSG)); //표준출력 기능을 가진 파일디스크립터 3을 가지고 출력
    exit(0);
}

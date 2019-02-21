#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ssu_employee.h"

#define DUMMY 0

int main(int argc, char *argv[])
{
    struct ssu_employee record;
    int fd;
    int flags;
    int length;
    int pid;

    if(argc < 2) {
        fprintf(stderr, "Usage : %s file\n", argv[0]);
        exit(1);
    }

    if((fd = open(argv[1], O_RDWR)) < 0) {
        fprintf(stderr, "open error for %s\n", argv[1]);
        exit(1);
    }

    if((flags = fcntl(fd, F_GETFL, DUMMY)) == -1) { //open해준 파일의 디스크립터를 통해 플래그들을 가져옴
        fprintf(stderr, "fcntl F_GETFL error\n");
        exit(1);
    }
    
    flags |= O_APPEND; //해당 플래그에 O_APPEND포함시킴

    if(fcntl(fd, F_SETFL, flags) == -1) { //O_APPEND가 포함된 플래그로 fd 파일 디스크립터 세팅해줌
        fprintf(stderr, "fcntl F_SETFL error\n");
        exit(1);
    }
    
    pid = getpid(); //현재 프로세스의 pid가져옴

    while(1) {
        printf("Enter employee name : ");
        scanf("%s", record.name);

        if(record.name[0] == '.')
            break;

        printf("Enter employee salary : ");
        scanf("%d", &record.salary);
        record.pid = pid;
        length = sizeof(record);

        if(write(fd, (char *)&record, length) != length) { //open시 O_APPEND 플래그를 포함시키지 않았지만 fcntl을 통해서 O_APPEND 플래그를 포함시켜줬으므로 파일의 EOF에 계속적으로 record가 추가됨을 볼 수 있다
            fprintf(stderr, "record write error\n");
            exit(1);
        }
    }

    close(fd);
    exit(0);
}

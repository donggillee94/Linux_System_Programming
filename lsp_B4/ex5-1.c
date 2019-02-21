#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int glob_val = 10;

int main()
{
    pid_t pid;
    int loc_val;

    loc_val = 100;
    fprintf(stdout, "before fork stdout|"); //버퍼링을 하기에 개행만나기전까지 출력안됨 자식 프로세스에게 상속됨
    fprintf(stderr, "before fork stderr|"); //버퍼링없이 바로 출력 한번만 출력됨

    if((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(1);
    }
    else if(pid == 0) {
        glob_val++;
        loc_val++;
    }
    else
        sleep(3);

    printf("pid = %d, glob_val = %d, loc_val = %d\n", getpid(), glob_val, loc_val);
    exit(0);
}

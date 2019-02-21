#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int flag;

    if((flag = fcntl(STDOUT_FILENO, F_DUPFD)) == -1) { //fcntl을 통해 DUP을 사용, 하지만 오류
        fprintf(stderr, "Error : Checking CLOSE_ON_EXEC\n");
        exit(1);
    }

    printf("CLOSE ON EXEC flag is = %d\n", flag);
    exit(0);
}

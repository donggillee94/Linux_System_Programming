#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_thread(void *arg);

int main()
{
    pthread_t tid;

    if(pthread_create(&tid, NULL, ssu_thread, NULL) != 0) { //쓰레드 생성
        fprintf(stderr, "pthread_create error\n");
        exit(1);
    }

    printf("%u\n", (unsigned int)tid); //생성된 쓰레드의 tid 출력

    if(pthread_create(&tid, NULL, ssu_thread, NULL) != 0) { //쓰레드 생성
        fprintf(stderr, "pthread_create error\n");
        exit(1);
    }

    printf("%u\n", (unsigned int)tid); //생성된 쓰레드의 tid 출력
    sleep(1);
    exit(0);
}

void *ssu_thread(void *arg) {
    pthread_t tid;

    tid = pthread_self(); //쓰레드가 수행하는 함수로써 생성된 쓰레드의 tid를 출력해주는 함수이다
    printf("->%u\n", (unsigned int)tid);
    return NULL;
}

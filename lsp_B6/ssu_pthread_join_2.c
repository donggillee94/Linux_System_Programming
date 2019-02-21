#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_thread1(void *arg);
void *ssu_thread2(void *arg);

int main()
{
    pthread_t tid1, tid2;

    if(pthread_create(&tid1, NULL, ssu_thread1, NULL) != 0) { //쓰레드생성
        fprintf(stderr, "pthread_create error\n");
        exit(1);
    }

    if(pthread_create(&tid2, NULL, ssu_thread2, NULL) != 0) { //쓰레드생성
        fprintf(stderr, "pthread_create error\n");
        exit(1);
    }

    printf("thread1의 리턴을 기다림\n");
    pthread_join(tid1, NULL); //첫번째 쓰레드가 종료될때까지 기다림 이 경우 thread1이 종료되면 main도 같이 종료됨
    exit(0);
}

void *ssu_thread1(void *arg) { //thread1이 수행하는 함수
    int i;

    for(i = 5; i != 0; i--) {
        printf("thread1 : %d\n", i);
        sleep(1);
    }

    printf("thread1 complete\n");
    return NULL;
}

void *ssu_thread2(void *arg) { //thread2가 수행하는 함수
    int i;
    for(i = 8; i != 0; i --) {
        printf("thread2 : %d\n", i);
        sleep(1);
    }
    printf("thread2 complete\n");
    return NULL;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_thread1(void *arg);
void *ssu_thread2(void *arg);

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

int count = 0;
int input = 0;
int t1 = 0, t2 = 0;

int main()
{
    pthread_t tid1, tid2;
    int status;

    if(pthread_create(&tid1, NULL, ssu_thread1, NULL) != 0) { //쓰레드1 생성
        fprintf(stderr, "pthread_create error\n");
        exit(1);
    }

    if(pthread_create(&tid2, NULL, ssu_thread2, NULL) != 0) { //쓰레드2 생성
        fprintf(stderr, "pthread_create error\n");
        exit(1);
    }

    while(1) {
        printf("2개 이상의 개수 입력 : ");
        scanf("%d", &input);

        if(input >= 2) {
            pthread_cond_signal(&cond1); //cond1에게 signal보냄
            break;
        }
    }

    pthread_join(tid1, (void *)&status);
    pthread_join(tid2, (void *)&status); //쓰레드 1,2가 종료될때까지 main쓰레드 대기

    printf("complete \n");
    exit(0);
}

void *ssu_thread1(void *arg) {
    while(1) {
        pthread_mutex_lock(&mutex1);

        if(input < 2) 
            pthread_cond_wait(&cond1, &mutex1); //cond1이 signal받을때까지 대기

        if(input == count) {
            pthread_cond_signal(&cond2); //cond2에 시그널보냄
            break;
        }

        if(count == 0) {
            t2++;
            count++;
            printf("Thread 1: %d\n", t1); //쓰레드1에서 t1출력
        }
        else if(count % 2 == 0) { //2의 배수일 경우에는
            t1 += t2; 
            count++;
            printf("Thread 1: %d\n", t1);
        }

        pthread_cond_signal(&cond2); //cond2에게 시그널보냄
        pthread_cond_wait(&cond1, &mutex1); //cond1이 signal받을때까지 대기
        pthread_mutex_unlock(&mutex1);
    }
    return NULL;
}

void *ssu_thread2(void *arg) {
    while(1) {
        pthread_mutex_lock(&mutex2);

        if(input < 2)//cond2기 시그널 받을때까지 대기
            pthread_cond_wait(&cond2, &mutex2);

        if(input == count) {
            pthread_cond_signal(&cond1);
            break;
        }

        if(count == 1) {
            count++;
            printf("Thread 2 : %d\n", t2);
        }
        else if(count % 2 == 1) {
            t2 += t1;
            count++;
            printf("Thread 2 : %d\n", t2);
        }

        pthread_cond_signal(&cond1); //cond1에게 시그널 보냄
        pthread_cond_wait(&cond2, &mutex2); //cond2가 시그널 받을때까지 대기
        pthread_mutex_unlock(&mutex2);
    }
    return NULL;
}

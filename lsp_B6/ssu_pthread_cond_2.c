#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int glo_val1 = 1, glo_val2 = 2;

void *ssu_thread1(void *arg);
void *ssu_thread2(void *arg);

int main()
{
    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, ssu_thread1, NULL);
    pthread_create(&tid2, NULL, ssu_thread2, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL); //main쓰레드가 thread1, thread2를 기다림
    pthread_mutex_destroy(&lock);//mutex 해제
    pthread_cond_destroy(&cond); //cond 해제

    exit(0);
}

void *ssu_thread1(void *arg) {
    sleep(1); //sleep하기에  쓰레드2가 먼저 호출
    glo_val1 = 2;
    glo_val2 = 1;

    if(glo_val1 > glo_val2) //thread2의 대기상태가 풀리면 이 문구 수행
        pthread_cond_broadcast(&cond); //thread2를 실행가능 상태로 만들어줌

    printf("ssu_thread1 end\n");
    return NULL;

}

void *ssu_thread2(void *arg) {
    struct timespec timeout;
    struct timeval now;

    pthread_mutex_lock(&lock);
    gettimeofday(&now, NULL);

    timeout.tv_sec = now.tv_sec + 5;
    timeout.tv_nsec = now.tv_usec * 1000;

    if(glo_val1 <= glo_val2) {
        printf("ssu_thread2 sleep\n");
        if(pthread_cond_timedwait(&cond, &lock, &timeout) == ETIMEDOUT) //해당 시간동안 대기 상태, thread1의 broadcast로 인해 timeout은 출력되지 않음
            printf("timeout\n");
        else
            printf("glo_val1 = %d, glo_val2 = %d\n", glo_val1, glo_val2);
    }

    pthread_mutex_unlock(&lock);

    printf("ssu_thread2 end\n");
    return NULL;
}

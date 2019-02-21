#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *ssu_thread(void *arg);

pthread_mutex_t lock;
int glo_val;

int main()
{
    int num1 = 1;
    int num2 = 2;
    pthread_t tid1, tid2;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&tid1, NULL, ssu_thread, (void *)&num1);
    pthread_create(&tid2, NULL, ssu_thread, (void *)&num2);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
}

void *ssu_thread(void *arg) {
    int num;
    while(glo_val < 10) {

        pthread_mutex_lock(&lock);

        if(*(int *)arg == 1 && glo_val <= 8) {
            glo_val++;
            printf("global value ssu_thread1 : ");
            printf("%d\n", glo_val);
        }
        else if(*(int *)arg == 2 && glo_val == 9){

            glo_val++;
            printf("global value ssu_thread2 : ");       
            printf("%d\n", glo_val);
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

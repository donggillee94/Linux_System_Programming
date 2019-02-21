#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_thread(void *arg);

pthread_t glo_tid;

int main()
{
	pthread_t loc_tid;

	if(pthread_create(&loc_tid, NULL, ssu_thread, NULL) != 0) { //쓰레드 생성
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}

	sleep(5); //5초간 sleep

	if(pthread_equal(loc_tid, glo_tid) == 0) { //두 tid가 다를경우 두 tid를 가진 쓰레드는 각자 서로 다른 쓰레드
		printf("다른 쓰레드\n");
		exit(0);
	}

	printf("동일한 쓰레드\n");
	exit(0);
}

void *ssu_thread(void *arg) {
	printf("쓰레드에서 자신의 쓰레드 ID를 전역변수에 할당 \n");
	glo_tid = pthread_self();
	return NULL;
}

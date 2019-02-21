#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *ssu_thread(void *arg);

int main()
{
	pthread_t tid;
	pid_t pid;

	if(pthread_create(&tid, NULL, ssu_thread, NULL) != 0) { //쓰레드 생성
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}

	pid = getpid(); //main의 pid
	tid = pthread_self(); //main 쓰레드의 tid
	printf("Main Thread: pid %u tid %u \n", (unsigned int)pid, (unsigned int)tid);
	sleep(1);
	exit(0);
}

void *ssu_thread(void *arg) {
	pthread_t tid;
	pid_t pid;

	pid = getpid();
	tid = pthread_self();
	printf("New Thread: pid %d tid %u \n", (int)pid, (unsigned int)tid); //생성된 쓰레드의 pid와 tid 출력
	return NULL;
}

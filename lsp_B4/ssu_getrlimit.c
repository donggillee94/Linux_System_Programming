#include <sys/resource.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ssu_show(limit) ssu_show_limit(limit, #limit)

void ssu_show_limit(int rlim_type, char *rlim_name);
struct timeval bgn, end;

int main()
{
	gettimeofday(&bgn, NULL);
	printf("	resource soft limit hard limit	\n\n");
	ssu_show(RLIMIT_CPU); //프로세스가 소비할 수 있는 최대 CPU 시간 
	ssu_show(RLIMIT_FSIZE); //파일 생성시 파일의 최대 크기
	ssu_show(RLIMIT_DATA); //자료 구역 전체의 최대 크기
	ssu_show(RLIMIT_STACK); //스택 최대 크기
	ssu_show(RLIMIT_CORE); //한 코어 파일의 최대 크기
#ifdef RLIMIT_RSS //메모리 상주 세트 크기 정의 시 해당 인자 함수에 넣어서 호출
	ssu_show(RLIMIT_RSS);
#endif
#ifdef RLIMIT_MEMLOCK //한 프로세스가 mlock으로 잠글 수 있는 메모리의 최대 용량 정의 시 해당 인자 함수에 넣어서 호출
	ssu_show(RLIMIT_MEMLOCK);
#endif
#ifdef RLIMIT_NPROC //한 프로세스가 열어둘 수 있는 최대 파일 개수 정의시 해당 인자 함수에 넣어서 호출
	ssu_show(RLIMIT_NPROC);
#endif
#ifdef RLIMIT_OFILE 
	ssu_show(RLIMIT_OFILE);
#endif
#ifdef RLIMIT_NOFILE //한 프로세스가 열어둘 수 있는 최대 파일 개수 정의되어있을 시 해당 인자 함수에 넣어서 호출
	ssu_show(RLIMIT_NOFILE);
#endif
#ifdef RLIMIT_VMEN //RLIMIT_AS와 동일하다
	ssu_show(RLIMIT_VMEN);
#endif

	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", ((end.tv_sec - bgn.tv_sec) * 1000000 + (end.tv_usec - bgn.tv_usec))%1000000);
	exit(0);
}

void ssu_show_limit(int rlim_type, char *rlim_name) {
	struct rlimit rlim;
	char cur[11], max[11];

	getrlimit(rlim_type, &rlim);//rlimit함수를 통해 해당인자의 limit 알아냄

	if(rlim.rlim_cur == RLIM_INFINITY)
		strcpy(cur, "infinity");
	else
		sprintf(cur, "%10ld", rlim.rlim_cur);

	if(rlim.rlim_max == RLIM_INFINITY)
		strcpy(max, "infinity");
	else
		sprintf(max, "%10ld", rlim.rlim_max);

	printf("%15s : %10s %10s\n", rlim_name, cur, max);
}

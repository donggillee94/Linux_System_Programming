#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void ssu_timestamp(char *str);

int main()
{
	unsigned int ret;

	ssu_timestamp("before sleep()"); //호출되는 시점의 시각 출력
	ret = sleep(10);  //sleep 수행
	ssu_timestamp("after sleep()"); //sleep 호출되고 나서의 시각 출력
	printf("sleep() returned %d\n", ret);
	exit(0);
}

void ssu_timestamp(char *str) {
	time_t time_val;
	
	time(&time_val);
	printf("%s the time is %s\n", str, ctime(&time_val));
}

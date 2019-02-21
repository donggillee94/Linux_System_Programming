#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>

void ssu_nested_func(int loc_var, int loc_volatile, int loc_register);

static jmp_buf glob_buffer;
struct timeval bgn, end;

int main()
{
	gettimeofday(&bgn, NULL);
	register int loc_register;
	register int loc_volatile;
	int loc_var;

	loc_var = 10;
	loc_volatile = 11;
	loc_register = 12;

	if(setjmp(glob_buffer) != 0) { //점프구간 설정, longjmp시 이쪽으로 구간 jump됨
		printf("after longjmp, loc_var = %d, loc_volatile = %d, loc_register = %d\n", loc_var, loc_volatile, loc_register);
		gettimeofday(&end, NULL);
		printf("elapsed time : %ld us\n", ((end.tv_sec - bgn.tv_sec) * 1000000 +(end.tv_usec - bgn.tv_usec))%1000000);
		exit(0);
	}

	loc_var = 80;
	loc_volatile = 81;
	loc_register = 82;
	ssu_nested_func(loc_var, loc_volatile, loc_register);// longjump함수 호출
	exit(0);
}

void ssu_nested_func(int loc_var, int loc_volatile, int loc_register)
{
	printf("before lonjmp, loc_var = %d, loc_volatime = %d, loc_register = %d\n", loc_var, loc_volatile, loc_register);
	longjmp(glob_buffer, 1);


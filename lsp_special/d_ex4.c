#include <stdio.h>
#include <unistd.h>
#include "d_ex3.c"

void f();

int main()
{
	int i;
	for(i = 1; i <= 20; i++) {
		DBGMSG("%d job processing", i);
		f();
	}
	printf("Completed\n");
	return 0;
}

void f()
{
	sleep(1);
}

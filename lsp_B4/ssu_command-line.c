#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

struct timeval bgn, end;

int main(int argc, char *argv[])
{
	gettimeofday(&bgn, NULL);

	int i;

	for(i = 0; i < argc; i++) 
		printf("argv[%d] : %s\n", i, argv[i]);
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", ((end.tv_sec - bgn.tv_sec)*1000000 + (end.tv_usec - bgn.tv_usec))%1000000);
	exit(0);
}

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

struct timeval bgn, end;

int main(int argc, char *argv[])
{
	gettimeofday(&bgn, NULL);
	extern char **environ;
	char **str;
	int i;

	for(i = 0; i < argc; i++)
		printf("argv[%d] : %s\n", i, argv[i]);

	for(str = environ; *str != 0; str++)
		printf("%s\n", *str);
	gettimeofday(&end, NULL);
	printf("elapsed time : %ls us\n", (end.tv_sec - bgn.tv_sec)*1000000+(end.tv_usec - bgn.tv_usec));
	exit(0);
}

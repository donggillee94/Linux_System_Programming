#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

void ssu_printenv(char *label, char ***envpp);
extern char **environ;
struct timeval bgn, end;

int main(int argc, char *argv[], char *envp[])
{
	gettimeofday(&bgn, NULL);
	ssu_printenv("Initially", &envp);
	putenv("TZ=PST8PDT");
	ssu_printenv("After changing TZ", &envp);
	putenv("WARNING=Don't use envp after putenv()");
	ssu_printenv("After setting a new variable", &envp);
	printf("value of WARNING is %s\n", getenv("WARNING"));
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", ((end.tv_sec - bgn.tv_sec)*1000000+(end.tv_usec - bgn.tv_usec))%1000000);
	exit(0);
}

void ssu_printenv(char *label, char ***envpp) {
	char **ptr;

	printf("----%s---\n", label);
	printf("envp is at %8o and contains %8o\n", envpp, *envpp);
	printf("environ is at %8o and contains %8o\n", &environ, environ);

	printf("My environment variable are:\n");

	for(ptr = environ; *ptr; ptr++)
		printf("(%8o) = %8o -> %s\n", ptr, *ptr, *ptr);

	printf("(%8o) = %8o\n", ptr, *ptr);
}

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void ssu_addone();

struct timeval bgn, end;
extern char **environ;
char glob_var[] = "HOBBY=swimming";

int main()
{
	gettimeofday(&bgn, NULL);
	int i;
	for(i = 0; environ[i] != NULL; i++)
		printf("environ[%d] : %s\n", i, environ[i]);

	putenv(glob_var);

	ssu_addone();
	printf("My hobby is %s\n", getenv("HOBBY"));
	printf("My lover is %s\n", getenv("LOVER"));
	strcpy(glob_var + 6, "fishing");

	for(i = 0; environ[i] != NULL; i++)
		printf("environ[%d] : %s\n", i, environ[i]);
	gettimeofday(&end, NULL);
	printf("elapsed time : %ld us\n", ((end.tv_sec - bgn.tv_sec)*1000000+(end.tv_usec - bgn.tv_usec))%1000000);
	exit(0);
}

void ssu_addone(void){
	char auto_var[10];

	strcpy(auto_var, "LOVER=js");
	putenv(auto_var);
}

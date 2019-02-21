#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH_MAX 1024

int main()
{
	char *pathname;

	if(chdir("/home/lee") < 0){
		fprintf(stderr, "chdir error\n");
		exit(1);
	}
	
	pathname = malloc(PATH_MAX);

	if(getcwd(pathname, PATH_MAX) == NULL) {
		fprintf(stderr, "getcwd error\n");
		exit(1);
	}
	printf("%s\n", pathname);
	exit(0);
}

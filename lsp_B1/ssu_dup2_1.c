#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	char *fname = "ssu_test.txt";
	int fd;

	if((fd = creat(fname, 0666)) < 0) {
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}
	
	printf("First printf is on the screen\n");
	dup2(fd, 1);
	printf("Second printf is in this file\n");
	exit(0);
}

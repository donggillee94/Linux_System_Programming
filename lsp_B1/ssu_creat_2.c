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
	else {
		close(fd);
		fd = open(fname, O_RDWR);
		printf("Succeeded!\n<%s> is new readable and writable\n", fname);
	}
	exit(0);
}

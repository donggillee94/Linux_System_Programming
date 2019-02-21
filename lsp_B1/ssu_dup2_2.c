#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main()
{
	char buf[BUFFER_SIZE];
	char *fname = "ssu_test.txt";
	int fd;
	int length;

	if((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	if(dup2(1,4) != 4) {
		fprintf(stderr, "dup2 error\n");
		exit(1);
	}

	while(1) {
		length = read(fd, buf, BUFFER_SIZE);

		if(length <= 0)
			break;

		write(1, buf, length);
	}
	exit(0);
}

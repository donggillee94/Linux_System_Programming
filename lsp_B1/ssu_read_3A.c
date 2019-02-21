#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	char c;
	int fd;
	char *fname = "ssu_test.txt";

	if((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	while(1) {
		if(read(fd, &c, 1) > 0) {
			putchar(c);
		}
		else
			break;
	}
	exit(0);
}

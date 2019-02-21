#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main()
{
	char *fname = "ssu_test.txt";
	int fd1, fd2;
	int count;
	char buf[BUFFER_SIZE];

	if((fd1 = open(fname, O_RDONLY, 0644)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	if((count = lseek(fd1, (off_t)0, SEEK_END)) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	printf("%d\n", count);
	lseek(fd1, (off_t)0, SEEK_SET);
	count = read(fd1, buf, BUFFER_SIZE);
	fd2 = dup(1);
	close(fd1);
	write(fd2, buf, count);
	exit(0);
}

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct timeval bgn, end;

int main(int argc, char *argv[])
{
	gettimeofday(&bgn, NULL);
	int fd;

	if(argc != 3) {
		fprintf(stderr, "Usage : %s <oldname> <newname>\n", argv[0]);
		exit(1);
	}

	if((fd = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "first open error for %s\n", argv[1]);
		exit(1);
	}
	else
		close(fd);

	if(rename(argv[1], argv[2]) < 0) {
		fprintf(stderr, "rename error\n");
		exit(1);
	}

	if((fd = open(argv[1], O_RDONLY)) < 0)
		printf("second open error for %s\n", argv[1]);
	else {
		fprintf(stderr, "it's very strange!\n");
		exit(1);
	}

	if((fd = open(argv[2], O_RDONLY)) < 0) {
		fprintf(stderr, "third open error for %s\n", argv[2]);
		exit(1);
	}
	
	printf("Everything is good!\n");
	gettimeofday(&end, NULL);
	printf("time : %ld us\n", end.tv_usec - bgn.tv_usec);
	exit(0);
}

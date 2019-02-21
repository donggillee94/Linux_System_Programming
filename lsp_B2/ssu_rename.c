#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd;

	if(argc != 3) {
		fprintf(stderr, "usage : %s oldname, newname\n", argv[0]);
		exit(1);
	}

	if((fd = open(argv[1], O_RDONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
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
	else
		printf("it's very strange\n");

	if((fd = open(argv[2], O_RDONLY)) < 0) {
		printf("third open error for %s\n", argv[2]);
		exit(1);
	}

	printf("Everything is good!\n");

	exit(0);
	}

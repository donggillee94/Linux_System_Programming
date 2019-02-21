#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define CREAT_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH

char buf1[] = "1234567890";
char buf2[] = "ABCDEFGHIJ";

int main()
{
	char *fname = "ssu_hole.txt";
	int fd;

	if((fd = creat(fname, CREAT_MODE)) < 0) {
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}

	if(write(fd, buf1, 12) != 12) {
		fprintf(stderr, "write error\n");
		exit(1);
	}
	if(lseek(fd, (off_t)15000, SEEK_SET) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}
	if(write(fd, buf2, 12) != 12) {
		fprintf(stderr, "write error\n");
		exit(1);
	}
	exit(0);
}

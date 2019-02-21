#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	char buf[64];
	char *fname = "ssu_tempfile";
	int fd;
	int length;

	if((fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0){
		fprintf(stderr, "first open error for %s\n", fname);
		exit(1);
	}

	if(unlink(fname) < 0) {
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}

	if(write(fd, "How are you?", 12) < 0) {
		fprintf(stderr, "write error for %s\n", fname);
		exit(1);
	}
	
	lseek(fd, (off_t)0, SEEK_SET);

	if((length = read(fd, buf, sizeof(buf))) < 0) {
		fprintf(stderr, "read error for %s\n", fname);
		exit(1);
	}

	buf[length] = '\0';

	printf("%s\n", buf);

	close(fd);

	if((fd = open(fname, O_RDWR)) < 0) {
		fprintf(stderr, "second open error for %s\n", fname);
		exit(1);
	}

	exit(0);
}

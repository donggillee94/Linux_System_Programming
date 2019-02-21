#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <utime.h>

int main(int argc, char *argv[])
{
	struct utimbuf time_buf;
	struct stat statbuf;
	int fd;
	int i;

	for(i = 1; i < argc; i++) {
		if(stat(argv[i], &statbuf) < 0) {
			fprintf(stderr, "stat error for %s\n", argv[i]);
			exit(1);
		}
		
		time_buf.actime = statbuf.st_atime;
		time_buf.modtime = statbuf.st_mtime;

		if((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) {
			fprintf(stderr, "open error for %s\n", argv[i]);
			continue;
		}

		if(utime(argv[i], &time_buf) < 0) {
			fprintf(stderr, "utime error for %s\n", argv[i]);
			continue;
		}
	}
	exit(0);
}

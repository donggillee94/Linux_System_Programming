#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct stat statbuf;

void ssu_checkfile(char *fname, time_t *intertime);

int main(int argc, char *argv[])
{
	time_t intertime;

	if(argc != 2) {
		fprintf(stderr, "Usage : %s file\n", argv[0]);
		exit(1);
	}
	if(stat(argv[1], &statbuf) < 0) {
		fprintf(stderr, "stat error for %s\n", argv[1]);
		exit(1);
	}

	intertime = statbuf.st_mtime;

	while(1) {
		ssu_checkfile(argv[1], &intertime);
		sleep(10);
	}
	exit(0);
}

void ssu_checkfile(char *fname, time_t *intertime)
{
	if(stat(fname, &statbuf) < 0) {
		fprintf(stderr, "Warning : ssu_checkfile() error!\n");
		exit(1);
	}
	else {
		if(*intertime != statbuf.st_mtime) {
			printf("Warning : %s was modified!.\n", fname);
			*intertime = statbuf.st_mtime;
		}
	}
}

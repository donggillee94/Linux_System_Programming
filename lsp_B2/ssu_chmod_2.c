#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define EXEC_MODE S_IXUSR | S_IXGRP | S_IXOTH

int main(int argc, char *argv[])
{
	struct stat statbuf;
	int i;

	if(argc < 2) {
		fprintf(stderr, "usage : %s file1 file2....\n", argv[0]);
		exit(1);
	}

	for(i = 1; i < argc; i++) {

		if(stat(argv[i], &statbuf) < 0) {
			fprintf(stderr, "stat error for %s\n", argv[i]);
			exit(1);
		}

		statbuf.st_mode |= EXEC_MODE;
		statbuf.st_mode ^= S_IXGRP | S_IXOTH;

		if(chmod(argv[i], statbuf.st_mode) < 0) 
			printf("%s : chmod error\n", argv[i]);
		else
			printf("%s : file permission was changed.\n", argv[i]);
	}

	exit(0);
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void print_file_types(struct stat *statbuf) {
	char *str;

	if(S_ISREG(statbuf -> st_mode)) {
		str = "regular";
	}
	else if(S_ISCHR(statbuf -> st_mode)) {
		str = "special character";
	}
	else if(S_ISDIR(statbuf -> st_mode)) {
		str = "directory";
	}
	else if(S_ISLNK(statbuf -> st_mode)) {
		str = "symbolic link";
	}
	else if(S_ISBLK(statbuf -> st_mode)) {
		str = "block special";
	}
	else if(S_ISFIFO(statbuf -> st_mode)) {
		str = "FIFO";
	}
	else if(S_ISSOCK(statbuf -> st_mode)) {
		str = "socket";
	}
	else
		str = "unknown mode";

	printf("%s\n", str);
}

int main(int argc, char *argv[])
{
	struct stat statbuf;
	int i;

	if(argc < 2) {
		fprintf(stderr, "usage : %s file1 file2 .....\n", argv[0]);
		exit(1);
	}
	for(i = 1; i < argc; i++) {
		if(lstat(argv[i], &statbuf) < 0) {
			fprintf(stderr, "lstat error for %s\n", argv[i]);
			exit(1);
		}
		else
			print_file_types(&statbuf);
	}

	exit(0);
}


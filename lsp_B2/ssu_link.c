#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if(argc < 2) {
		fprintf(stderr, "usage : %s file1 file2\n", argv[0]);
		exit(1);
	}

	if(link(argv[1], argv[2]) < 0) {
		fprintf(stderr, "link error for %s %s\n", argv[1], argv[2]);
		exit(1);
	}

	exit(0);
}

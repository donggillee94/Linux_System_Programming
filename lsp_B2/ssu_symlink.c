#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if(argc != 3) {
		fprintf(stderr, "usage : %s actualpath sympath\n", argv[0]);
		exit(1);
	}

	if(symlink(argv[1], argv[2]) < 0) {
		fprintf(stderr, "symlink error\n");
		exit(1);
	}
	else
		printf("symlink : %s -> %s\n", argv[2], argv[1]);

	exit(0);
}

